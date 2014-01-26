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

void ClientWrapper::deliverMessage(std::string& msg)
{
    auxilo::Message mesg;
    mesg.ParseFromString(msg);

    // If observer is registered, inform it about the mesg.
    if(observer_ != nullptr)
    {
        observer_->update(mesg);
        return;
    }

    // Otherwise store the message in buffer.
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
        auxilo::HelloRequest initializer;
        initializer.set_customerid(customerID);
        initializer.set_devicename(deviceID);
        initializer.set_isbox(isBox);

        std::string mesg;
        initializer.SerializeToString(&mesg);
        Client_->sendMessage(mesg, PROTOCOL_INITIALIZATION_MESSAGE);


        std::cout << "Client initialized." << std::endl;

        return true;

    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

void ClientWrapper::sendDeviceList(auxilo::DeviceList& msg)
{
    std::string pntr;
    msg.SerializeToString(&pntr);
    Client_->sendMessage(pntr, PROTOCOL_INITIALIZATION_MESSAGE);
}

void ClientWrapper::sendMessage(auxilo::Message &msg)
{
    std::cout << "Sending DataMessage." << std::endl;
    std::string pntr;
    msg.SerializeToString(&pntr);
    Client_->sendMessage(pntr, PROTOCOL_NORMAL_MESSAGE);
}

auxilo::Message *ClientWrapper::getLastMessage()
{
    auxilo::Message* tmp = &receivedMessages_.front();
    receivedMessages_.pop_front();
    return tmp;
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
