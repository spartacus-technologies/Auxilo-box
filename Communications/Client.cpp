#include "Client.hpp"
#include "ClientWrapper.hpp"

#include <stdlib.h> // atoi


Client::Client(boost::asio::io_service &io_service,
               boost::asio::ip::tcp::resolver::iterator endpoint_iterator, ClientWrapper *wrapper):
    io_service_(io_service), socket_(io_service), wrapper_(wrapper)
{
    do_connect(endpoint_iterator);
}

Client::~Client()
{
}

void Client::do_connect(boost::asio::ip::tcp::resolver::iterator
                        endpoint_iterator)
{

   boost::asio::async_connect(socket_, endpoint_iterator,
       [this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::
                              iterator)
   {

      if (!ec)
      {
          // Read message header.
          do_read_header();
      }
      else
          std::cout << "Error: " << ec << std::endl;
   });
}

bool Client::initiateConnection(std::string& msg)
{

    SocketMessage message;
    message.setType(0);
    message.setLength(msg.size());
    std::memcpy(message.dataBuffer(), msg.c_str(), msg.length());
    message.encodeHeader();
    boost::system::error_code ec;
    boost::asio::write(socket_, boost::asio::buffer(message.headerBuffer(),
                                                    message.getLength()+ SocketMessage::HEADER_LENGTH),
        boost::asio::transfer_all(), ec);

    // Wait for ACK
    // TODO
    return true;
}


void Client::end_session()
{
    std::cout << "Terminating server session." << std::endl;
    SocketMessage message;
    message.setType(1000);
    message.encodeHeader();


    bool containerEmpty = false;
    {
        std::lock_guard<std::mutex> guard(msgBufferMutex_);
        containerEmpty = msgBuffer_.empty();
        msgBuffer_.push_back(message);
    }

    if (containerEmpty)
    {
        do_write();
    }
}

void Client::do_read_header()
{
    boost::asio::async_read(socket_,
           boost::asio::buffer(message_receive_.headerBuffer(), SocketMessage::
                               HEADER_LENGTH),
       [this](boost::system::error_code ec, std::size_t /*length*/)
       {
         if (!ec)
         {
             // Decode received header
             message_receive_.decodeHeader();
             // Check for connection terminate messages
             if( message_receive_.getType() == 1000 )
             {
                 // Terminate this session
                 // TODO
                 //socket_.close();
             }
             // Check for ACK
             else if( message_receive_.getType() == 10 )
             {

             }

             do_read_data();
         }
         else
         {
           socket_.close();
         }
       });
}

void Client::do_read_data()
{

    boost::asio::async_read(socket_,
            boost::asio::buffer(message_receive_.dataBuffer(), message_receive_.getLength()),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    // Data received
                    std::string message = message_receive_.dataBuffer();
                    wrapper_->deliverMessage(message);
                    do_read_header();
                }
                else
                {
                    // TODO: Drop
                    std::cerr << "Error " << ec << std::endl;
                }
            });
}


void Client::sendMessage(std::string msg, int32_t type)
{

    // Todo: check message memory boundaries, 512 bytes max.

    SocketMessage message;
    message.setLength(msg.length());
    std::memcpy(message.dataBuffer(), msg.c_str(), msg.length());
    message.setType(type);
    message.encodeHeader();


    bool containerEmpty = false;
    {
        std::lock_guard<std::mutex> guard(msgBufferMutex_);
        containerEmpty = msgBuffer_.empty();
        msgBuffer_.push_back(message);
    }

    if (containerEmpty)
    {
        do_write();
    }
}

void Client::do_write()
{
    boost::asio::async_write(socket_,
                             boost::asio::buffer(msgBuffer_.front().headerBuffer(),
                                                 msgBuffer_.front().getLength()+ SocketMessage::HEADER_LENGTH),
        [this](boost::system::error_code ec, std::size_t)
        {
          if (!ec)
          {
              std::lock_guard<std::mutex> guard(msgBufferMutex_);
              std::cout << "sending message with length " << msgBuffer_.front().getLength() << std::endl;


              std::cout << "Print the message:" << std::endl;
              for( unsigned i = 0; i < msgBuffer_.front().getLength() + 8; ++i)
              {
                  int ij = static_cast<int> (msgBuffer_.front().headerBuffer()[i]);
                  std::cout << "char[" << i << "]: " << msgBuffer_.front().headerBuffer()[i] << " (" << ij << ")" << std::endl;
              }

              msgBuffer_.pop_front();

              if (!msgBuffer_.empty())
              {
                  do_write();
              }
          }
          else
          {
              std::cerr << "Error: writing to socket " << ec << std::endl;
              socket_.close();
          }
        });
}
