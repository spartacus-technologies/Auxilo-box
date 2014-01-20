#ifndef CLIENTWRAPPER_HPP
#define CLIENTWRAPPER_HPP

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "protocol.pb.h"
#include "Client.hpp"

class ClientObserver;

class ClientWrapper
{
    public:
        // Server IP or name, and port as parameters
        ClientWrapper(std::string ip, std::string port);
        ~ClientWrapper();

        // Initialize connection to server. Returns false if name not resolvable
        // TODO: Should return false also if server does not respond.
        bool initiateConnection(std::string& customerID,
                                std::string& deviceID, bool isBox);        
        // For sending DataMessage
        void sendMessage(DataMessage &msg);

        // Get the last message from FIFO buffer. Only this or Observer method
        // should be used, NOT simultaneously.
        bool getLastMessage(DataMessage& rcv);

        // Add observer, which gets notifys when message has been received.
        void addObserver(ClientObserver* observer);

        // Update device list
        // TODO

        // Call back function from Client.
        void deliverMessage(std::string &msg);

    private:
        std::unique_ptr<Client> Client_;
        std::string serverIP_;
        std::string serverPort_;
        boost::asio::io_service io_service_;
        std::thread t_;

        ClientObserver* observer_;

        std::mutex rcvMsgMutex_;

        //FIFO-type round buffer for storing messages send by server.
        std::deque<DataMessage> receivedMessages_;

};

#endif // CLIENTWRAPPER_HPP
