#ifndef CLIENT_HPP
#define CLIENT_HPP


#include <boost/asio.hpp>
#include "protocol.pb.h"
#include "SocketMessage.hpp"

#include <mutex>
#include <deque>

class ClientWrapper;

class Client
{
    public:


        Client(boost::asio::io_service& io_service,
               boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
               ClientWrapper* wrapper);
        ~Client();

        // Makes the initial hand shake with the server. Parameter need to
        // be valid HelloRequest message.
        bool initiateConnection(std::string& msg);

        // Terminates this session with server.
        void end_session();

        // Send serialized message to server
        void sendMessage(std::string msg, int32_t type);

    private:

        // Initiate connection to server
        void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

        // Assynchronically write data
        void do_write();

        // Assynchronically read header
        void do_read_header();

        // Assynchronically read data
        void do_read_data();

        boost::asio::io_service& io_service_;

        boost::asio::ip::tcp::socket socket_;

        // For storing messages to be sent
        std::deque<SocketMessage> msgBuffer_;

        // Temporary message for receiving
        SocketMessage message_receive_;

        // Pointer to ClientWrapper for callbacks
        ClientWrapper* wrapper_;

        std::mutex msgBufferMutex_;
};

#endif // CLIENT_HPP
