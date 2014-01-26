#include "SocketMessage.hpp"

#include <iostream>
#include <cstring>

SocketMessage::SocketMessage() :
    length_(0),
    type_(0),
    data_()
{
}

SocketMessage::SocketMessage(uint32_t tpe, std::string msg):
    length_(msg.length()),type_(tpe)
{
    std::memcpy(dataBuffer(), msg.c_str(), msg.length());

}


SocketMessage::~SocketMessage()
{
}

char* SocketMessage::headerBuffer()
{
    return data_;
}

char* SocketMessage::dataBuffer()
{
    return &data_[HEADER_LENGTH];
}

uint32_t SocketMessage::getLength() const
{
    return length_;
}

void SocketMessage::setLength(const uint32_t length)
{
    length_ = length;
}

uint32_t SocketMessage::getType() const
{
    return type_;
}

void SocketMessage::setType(const uint32_t type)
{
    type_ = type;
}

void SocketMessage::decodeHeader()
{
    // Change header to Little endian byte order
    changeEndianess();

    length_ = *(reinterpret_cast <uint32_t *> (data_));
    type_ = *(reinterpret_cast <uint32_t *> (&data_[MESSAGELENGTH_LENGTH]));
}

void SocketMessage::encodeHeader()
{
    std::memcpy(data_, &length_, MESSAGELENGTH_LENGTH);
    std::memcpy(&data_[MESSAGELENGTH_LENGTH], &type_, TYPE_LENGTH);

    // Change header to Big endian byte order
    changeEndianess();
}

void SocketMessage::changeEndianess()
{
    int i = 0;
    int j = 3;
    char tmp;

    // Change endiadess of both blocks in the header (type and lenght).
    while( i < 2 )
    {
        tmp = data_[j];
        data_[j] = data_[i];
        data_[i] = tmp;

        tmp = data_[j + 4];
        data_[j + 4] = data_[i + 4];
        data_[i + 4] = tmp;

        i++;
        j--;
    }
}

