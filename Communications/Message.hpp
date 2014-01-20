#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <stdint.h>
#include <string>

class Message
{
    public:
        // TODO: Endianess!

        enum { MESSAGELENGTH_LENGTH = 4 };
        enum { TYPE_LENGTH = 4 };
        enum { HEADER_LENGTH = MESSAGELENGTH_LENGTH + TYPE_LENGTH };

        // TODO: Change to bigger
        enum { MAX_BODY_LENGTH = 512 };

        Message();

        Message(uint32_t tpe, std::string msg);

        virtual ~Message();

        // Get pointer to beginning of header
        char* headerBuffer();

        // Get pointer to beginning of data
        char* dataBuffer();

        // Get message length
        uint32_t getLength() const;

        // Set message length
        void setLength(const uint32_t length);

        // Get message type
        uint32_t getType() const;

        // Set message type
        void setType(const uint32_t type);

        /// Encode header into raw data
        void encodeHeader();

        /// Decode header from raw data
        void decodeHeader();

    private:
        uint32_t length_;
        uint32_t type_;
        char data_[HEADER_LENGTH + TYPE_LENGTH + MAX_BODY_LENGTH];
};

#endif /* MESSAGE_HPP */
