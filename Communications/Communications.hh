#ifndef COMMUNICATIONS_HH
#define COMMUNICATIONS_HH

#include <string>
#include <memory>

const std::string SERVERIPADDRESS = "95.85.11.190";
const std::string PORT = "8081";

#include "protocol.pb.h"

class ClientWrapper;

class Communications
{
    public:
        Communications();
        ~Communications();

        void initiate(protobuf::DeviceList list);

        void sendMessage(protobuf::Message& msg);

        bool getMessage();

    private:

        std::unique_ptr<ClientWrapper> clientWrapper_;
};

#endif // COMMUNICATIONS_HH



