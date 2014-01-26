#ifndef CLIENTWRAPPER_HPP
#define CLIENTWRAPPER_HPP

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "protocol.pb.h"
#include "Client.hpp"

int32_t const PROTOCOL_INITIALIZATION_MESSAGE = 0;
int32_t const PROTOCOL_NORMAL_MESSAGE = 1;

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

        // Sending a DeviceList to server is a part of the handshake.
        void sendDeviceList(auxilo::DeviceList &msg);

        // For sending DataMessage
        void sendMessage(auxilo::Message &msg);

        // Get the last message from FIFO buffer. Only this or Observer method
        // should be used, NOT simultaneously.
        auxilo::Message* getLastMessage();

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

        //FIFO-type round buffer for storing messages send by server.
        std::deque<auxilo::Message> receivedMessages_;

};

#endif // CLIENTWRAPPER_HPP
