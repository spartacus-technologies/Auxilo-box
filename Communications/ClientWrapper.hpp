#ifndef CLIENTWRAPPER_HPP
#define CLIENTWRAPPER_HPP

#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <mutex>

#include "Protocol.pb.h"
#include "Client.hpp"

uint32_t const PROTOCOL_INIT_MESSAGE = 0;
uint32_t const PROTOCOL_NORMAL_MESSAGE = 1;
uint32_t const PROTOCOL_ACK_MESSAGE = 10;
uint32_t const PROTOCOL_FIN_MESSAGE = 1000;

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
        bool sendDeviceList(auxilo::DeviceList &msg);

        // For sending DataMessage
        bool sendMessage(auxilo::Message &msg);

        // Get the last message from FIFO buffer. Only this or Observer method
        // should be used, NOT simultaneously.
        bool getLastMessage(auxilo::Message &msg);

        // Add observer, which gets notifys when message has been received.
        void addObserver(ClientObserver* observer);

        // Call back function from Client.
        void deliverMessage(std::string &msg, uint32_t type);

    private:

        std::unique_ptr<Client> client_;
        std::string serverIP_;
        std::string serverPort_;
        boost::asio::io_service io_service_;
        std::thread t_;

        ClientObserver* observer_;

        std::mutex msgDequeMutex_;

        //FIFO-type round buffer for storing messages send by server.
        std::deque<auxilo::Message> receivedMessages_;

        enum SessionState { Started,
                            HelloRequest,
                            DeviceList,
                            HandshakeComplete,
                            Finished
                          };

        SessionState state_;

        bool isBox_;
};

#endif // CLIENTWRAPPER_HPP
