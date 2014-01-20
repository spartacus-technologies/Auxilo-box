#include "ClientWrapper.hpp"
#include "ClientObserver.hpp"

ClientWrapper::ClientWrapper(std::string ip, std::string port):
    serverIP_(ip), serverPort_(port), observer_(nullptr)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ClientWrapper::~ClientWrapper()
{
    Client_->end_session();

    if( t_.joinable())
    {
        t_.join();
    }
}

// Call back function
void ClientWrapper::deliverMessage(std::string& msg)
{
    DataMessage mesg;
    mesg.ParseFromString(msg);

    // If observer is registered, inform it about the mesg.
    if(observer_ != nullptr)
    {
        observer_->update(mesg);
        return;
    }

    // Otherwise store the message in buffer.
    std::lock_guard<std::mutex> guard(rcvMsgMutex_);
    receivedMessages_.push_back(mesg);
}

bool ClientWrapper::initiateConnection(std::string& customerID,
                                       std::string& deviceID, bool isBox)
{

    try
    {
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(serverIP_, serverPort_);
        auto endpoint_iterator = resolver.resolve(query);

        Client_.reset(new Client(io_service_,endpoint_iterator, this));

        t_ = std::thread([this]()
            {
                io_service_.run();
            });

        // Inform server that this box/client wants to join customer room by
        // sending Hellorequest message.
        HelloRequest initializer;
        initializer.set_customerid(customerID);
        initializer.set_devicename(deviceID);
        initializer.set_isbox(isBox);
        std::cout << customerID << deviceID << isBox << std::endl;
        std::string mesg;
        initializer.SerializeToString(&mesg);
        Client_->sendMessage(mesg);


        std::cout << "Client initialized." << std::endl;

        return true;

    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

void ClientWrapper::sendMessage(DataMessage& msg)
{
    std::string pntr;
    if(!msg.SerializeToString(&pntr))
    {
        std::cout << "Ei onnistu" << std::endl;
    }
    Client_->sendMessage(pntr);
}

bool ClientWrapper::getLastMessage(DataMessage &rcv)
{
    {
        std::lock_guard<std::mutex> guard(rcvMsgMutex_);
        if(receivedMessages_.empty())
        {
            return false;
        }
        rcv = receivedMessages_.front();
        receivedMessages_.pop_front();
    }
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
