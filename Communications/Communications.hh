#ifndef COMMUNICATIONS_HH
#define COMMUNICATIONS_HH

#include <string>
#include <memory>

const std::string SERVERIPADDRESS = "95.85.11.190";
const std::string PORT = "8081";

#include "Protocol.pb.h"

class ClientWrapper;

class Communications
{
    public:
        Communications();
        ~Communications();

        void initiate(auxilo::DeviceList list);

        void sendMessage(auxilo::Message& msg);

        bool getMessage(auxilo::Message &msg);

    private:

        std::unique_ptr<ClientWrapper> clientWrapper_;
};

#endif // COMMUNICATIONS_HH



