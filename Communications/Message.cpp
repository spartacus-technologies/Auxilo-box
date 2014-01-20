#include "Message.hpp"

#include <iostream>
#include <cstring>

Message::Message() :
    length_(0),
    type_(0),
    data_()
{
}

Message::Message(uint32_t tpe, std::string msg):
    length_(msg.length()),type_(tpe)
{
    std::memcpy(dataBuffer(), msg.c_str(), msg.length());

}


Message::~Message()
{
}

char* Message::headerBuffer()
{
    return data_;
}

char* Message::dataBuffer()
{
    return &data_[HEADER_LENGTH];
}

uint32_t Message::getLength() const
{
    return length_;
}

void Message::setLength(const uint32_t length)
{
    length_ = length;
}

uint32_t Message::getType() const
{
    return type_;
}

void Message::setType(const uint32_t type)
{
    type_ = type;
}

void Message::decodeHeader()
{
    length_ = *(reinterpret_cast <uint32_t *> (data_));
    type_ = *(reinterpret_cast <uint32_t *> (&data_[MESSAGELENGTH_LENGTH]));
}

void Message::encodeHeader()
{
    std::memcpy(data_, &length_, MESSAGELENGTH_LENGTH);
    std::memcpy(&data_[MESSAGELENGTH_LENGTH], &type_, TYPE_LENGTH);
}

