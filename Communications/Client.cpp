#include "Client.hpp"
#include "ClientWrapper.hpp"


Client::Client(boost::asio::io_service &io_service,
               boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
               ClientWrapper *wrapper):
    socket_(io_service), wrapper_(wrapper)
{
    do_connect(endpoint_iterator);
}

Client::~Client()
{
    std::cout << "Closing socket." << std::endl;
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
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
      {
          std::cout << "Error: " << ec << std::endl;
          wrapper_->terminate();
      }
   });
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

             do_read_data();
         }
         else
         {
           wrapper_->terminate();
         }
       });
}

void Client::do_read_data()
{

    boost::asio::async_read(socket_,
            boost::asio::buffer(message_receive_.dataBuffer(),
                                message_receive_.getLength()),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    // Data received
                    std::string tmp;
                    for(unsigned i = 0; i < message_receive_.getLength(); ++i)
                    {
                        tmp += message_receive_.dataBuffer()[i];
                    }
                    std::cout << "received message with length " <<
                                 message_receive_.getLength() << std::endl;

                    wrapper_->deliverMessage(tmp, message_receive_.getType());
                    do_read_header();
                }
                else
                {
                    // TODO: Drop
                    std::cerr << "Error " << ec << std::endl;
                    wrapper_->terminate();
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
    boost::asio::async_write(socket_, boost::asio::buffer(msgBuffer_.front().
                             headerBuffer(), msgBuffer_.front().getLength() +
                             SocketMessage::HEADER_LENGTH),
        [this](boost::system::error_code ec, std::size_t)
        {
          if (!ec)
          {
              std::lock_guard<std::mutex> guard(msgBufferMutex_);
              std::cout << "sending message with length " << msgBuffer_.
                           front().getLength() << std::endl;


              msgBuffer_.pop_front();

              if (!msgBuffer_.empty())
              {
                  do_write();
              }
          }
          else
          {
              std::cerr << "Error: writing to socket " << ec << std::endl;
              wrapper_->terminate();
          }
        });
}
