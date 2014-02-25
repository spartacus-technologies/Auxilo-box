#include "ClientWrapper.hpp"
#include "ClientObserver.hpp"

#include <exception>

ClientWrapper::ClientWrapper(std::string ip, std::string port):
    serverIP_(ip), serverPort_(port), observer_(nullptr),
    state_(Started), isBox_(false)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ClientWrapper::~ClientWrapper()
{
    // TODO Client_->end_session();
    io_service_.stop();
    if( t_.joinable())
    {
        t_.join();
    }
}

// Handles incoming messages from server depending on the state of the session.
void ClientWrapper::deliverMessage(std::string& msg, uint32_t type)
{
    switch(type)
    {
        case PROTOCOL_INIT_MESSAGE:
        {
            // Server sent a DeviceList message.
            auxilo::DeviceList mesg;
            if(!mesg.ParseFromString(msg))
            {
                std::cout << "Parse error: Message in wrong format. Could "
                          << "not parse to a protobuf message." << std::endl;
                // TODO: throw exception.
            }
            else
            {
                std::cout << "Received DeviceList, it contains: " << std::endl;
                for(int i = 0; i <  mesg.devicelist_size(); ++i)
                {
                    auxilo::DeviceInfo info = mesg.devicelist(i);
                    std::cout << "DeviceInfo contains: BoxID" << info.boxid() <<
                                 ", deviceID: " << info.deviceid() << ", alias: "
                              << info.aliasname() << ", description: " <<
                                 info.description() << ", type: " << info.type()
                              << "." << std::endl;
                }
                std::cout << "End of DeviceList." << std::endl;

                // Send ACK to server.
                std::string ack;
                client_->sendMessage(ack, PROTOCOL_ACK_MESSAGE);

                std::lock_guard<std::mutex> guard(libnetworkMutex_);
                state_ = HandshakeComplete;
            }
            break;
        }
        case PROTOCOL_ACK_MESSAGE:
        {
            std::lock_guard<std::mutex> guard(libnetworkMutex_);
            // Server acknowledged either the HelloRequest or DeviceList message.
            if(state_ == HelloRequest)
            {
                std::cout << "First ACK." << std::endl;
                // First ACK from server.
                if(!isBox_)
                {
                    // In case of a client. Server ACK equals to "handshake
                    // complete" according to protocol definition.
                    state_ = HandshakeComplete;
                    // Allow sending messages.
                }
                else
                {
                    // HelloRequest message was approved by the server.
                    state_ = DeviceList;
                }
            }
            else if(state_ == DeviceList)
            {
                // Second ACK from server. DeviceList was approved.
                state_ = HandshakeComplete;

                std::cout << "Second ACK." << std::endl;
            }
            else
            {
                // Should not happen..
                std::cout << "Server sent an odd ACK." << std::endl;
            }
        break;
        }

        case PROTOCOL_NORMAL_MESSAGE:
        {
            {
                std::lock_guard<std::mutex> guard(libnetworkMutex_);
                if(state_ != HandshakeComplete)
                {
                    std::cout << "Sessions in server and client appear to be unsynced."
                              << std::endl;
                }
            }
            auxilo::Message mesg;
            if(!mesg.ParseFromString(msg))
            {
                std::cout << "Parse error: Message in wrong format. Could "
                          << "not parse to a protobuf message." << std::endl;
            }
            else
            {
                mesg.ParseFromString(msg);

                // If observer is registered, inform it about the mesg.
                if(observer_ != nullptr)
                {
                    observer_->update(mesg);
                    return;
                }

                // Otherwise store the message in buffer.
                std::lock_guard<std::mutex> guard(libnetworkMutex_);
                receivedMessages_.push_back(mesg);
            }
            break;
        }
        case PROTOCOL_FIN_MESSAGE:
        {
            std::cout << "Received terminate message." << std::endl;
            std::lock_guard<std::mutex> guard(libnetworkMutex_);
            state_ = Finished;
            break;
        }
    }
}

bool ClientWrapper::initiateConnection(std::string& customerID,
                                       std::string& deviceID, bool isBox)
{
    if(state_ == Started)
    {
        try
        {
            isBox_ = isBox;
            boost::asio::ip::tcp::resolver resolver(io_service_);
            boost::asio::ip::tcp::resolver::query query(serverIP_, serverPort_);
            auto endpoint_iterator = resolver.resolve(query);

            client_.reset(new Client(io_service_, endpoint_iterator, this));

            t_ = std::thread([this]()
            {
                io_service_.run();
            });

            // Inform server that this box/client wants to join customer room by
            // sending Hellorequest message.
            auxilo::HelloRequest initializer;
            initializer.set_customerid(customerID);
            initializer.set_devicename(deviceID);
            initializer.set_isbox(isBox);

            std::string mesg;
            initializer.SerializeToString(&mesg);
            client_->sendMessage(mesg, PROTOCOL_INIT_MESSAGE);

            std::lock_guard<std::mutex> guard(libnetworkMutex_);
            state_ = HelloRequest;

            // Client now waits for a ACK from server.

            return true;

        }
        catch(std::exception &e)
        {
            state_= Finished;
        }
    }
    else
    {
        // Can not initialize connection if already initialized.

    }
    return false;
}



bool ClientWrapper::sendDeviceList(auxilo::DeviceList& msg)
{
    std::lock_guard<std::mutex> guard(libnetworkMutex_);
    if(state_ == HelloRequest || state_ == DeviceList)
    {
        std::string pntr;
        msg.SerializeToString(&pntr);
        client_->sendMessage(pntr, PROTOCOL_INIT_MESSAGE);
        return true;
    }
    return false;
}


bool ClientWrapper::sendMessage(auxilo::Message &msg)
{
    std::lock_guard<std::mutex> guard(libnetworkMutex_);
    if(state_ == HandshakeComplete)
    {
        // Communication ready and messages can be sent to server.
        std::string pntr;
        msg.SerializeToString(&pntr);
        client_->sendMessage(pntr, PROTOCOL_NORMAL_MESSAGE);
        return true;
    }
    return false;
}



bool ClientWrapper::getLastMessage(auxilo::Message &msg)
{
    std::lock_guard<std::mutex> guard(libnetworkMutex_);
    if(state_ == Finished || receivedMessages_.empty())
    {
        return false;
    }

    msg = receivedMessages_.front();
    receivedMessages_.pop_front();
    return true;
}

void ClientWrapper::addObserver(ClientObserver *observer)
{
    if( observer_ != nullptr )
    {
        //TODO
        std::cerr << "Adding observer for the second time!" << std::endl;
    }

    observer_ = observer;
}

bool ClientWrapper::connectionStatus ()
{
    std::lock_guard<std::mutex> guard(libnetworkMutex_);
    return state_ == HelloRequest || state_ == DeviceList ||
           state_ == HandshakeComplete;
}

void ClientWrapper::terminate()
{
    std::lock_guard<std::mutex> guard(libnetworkMutex_);
    state_ = Finished;
}
