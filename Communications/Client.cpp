#include "Client.hpp"
#include "ClientWrapper.hpp"


Client::Client(boost::asio::io_service &io_service,
               boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
               ClientWrapper *wrapper): timerState_(IdleWait),
    socket_(io_service), timer_(io_service), wrapper_(wrapper)
{
    do_connect(endpoint_iterator);
}

Client::~Client()
{
    std::cout << "Closing socket." << std::endl;
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
            // Set heartbeat ack intervall to 15min.
            resetTimer();

            do_read_header();
        }
        else
        {
            wrapper_->terminate();
            socket_.close();
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

            // Reset timer.
            resetTimer();

            switch (message_receive_.getType())
            {
                case PROTOCOL_HBACK_MESSAGE:
                {
                    // Send heartbeat ack back to client as acknowledgement if
                    // in IdleWait state.
                    if(timerState_ == IdleWait)
                    {
                        std::string tmp;
                        sendMessage(tmp, PROTOCOL_HBACK_MESSAGE);
                    }
                    // Otherwise the HB ack is a response to hb ack send by
                    // this instance.
                    else
                    {
                        timerState_ = IdleWait;
                    }
                    std::cout << "HB ack" << std::endl;
                    do_read_header();
                    break;
                }
                case PROTOCOL_INIT_MESSAGE:
                case PROTOCOL_NORMAL_MESSAGE:
                case PROTOCOL_ACK_MESSAGE:
                case PROTOCOL_FIN_MESSAGE:
                {
                    do_read_data();
                    break;
                }
                default:
                {
                    // TODO
                    break;
                }
            }
        }
        else
        {
            if(ec == boost::asio::error::operation_aborted)
            {
                // Ignored. socket_.close() causes operation_aborted error.
            }
            else
            {
                wrapper_->terminate();
                socket_.close();
            }
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
            std::string tmp(message_receive_.dataBuffer(),
                            message_receive_.getLength());

            wrapper_->deliverMessage(tmp, message_receive_.getType());
            do_read_header();
        }
        else
        {
            wrapper_->terminate();
            socket_.close();
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
            // Reset timer if not HB ack message.
            if(msgBuffer_.front().getType() != PROTOCOL_HBACK_MESSAGE)
            {
                resetTimer();
            }

            std::lock_guard<std::mutex> guard(msgBufferMutex_);


            msgBuffer_.pop_front();

            if (!msgBuffer_.empty())
            {
                do_write();
            }
        }
        else
        {
            if(ec == boost::asio::error::operation_aborted)
            {
                // Ignored.
            }
            else
            {
                wrapper_->terminate();
                socket_.close();
            }
        }
    });
}

void Client::HB_handler(const boost::system::error_code &code)
{
    if(code == boost::asio::error::operation_aborted)
    {
        // Ignored.
    }
    else if(timerState_ == IdleWait)
    {
        // Send heartbeat ack.
        std::string tmp;
        sendMessage(tmp, PROTOCOL_HBACK_MESSAGE);

        // Reset timer to give client 60s to reply.
        timer_.expires_from_now(boost::posix_time::seconds(60));
        timer_.async_wait(std::bind(&Client::HB_handler, this,
                                    std::placeholders::_1));
        timerState_ = ResponseWait;
        std::cout << "Timer reset to 60s." << std::endl;
    }
    else if(timerState_ == ResponseWait)
    {
        // If this expires, session is dead -> drop.
        wrapper_->terminate();
        socket_.close();
        std::cout << "Timer: end session." << std::endl;
    }
}

void Client::resetTimer()
{
    // Set heartbeat ACK intervall to 15min.
    timer_.expires_from_now(boost::posix_time::seconds(900));
    timer_.async_wait(std::bind(&Client::HB_handler, this,
                                std::placeholders::_1));
}
