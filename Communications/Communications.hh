#ifndef COMMUNICATIONS_HH
#define COMMUNICATIONS_HH

#include <string>
#include <memory>

const std::string SERVERIPADDRESS = "94.237.68.33";
const std::string PORT = "8081";

#include "protocol.pb.h"

class ClientWrapper;

class Communications
{
    public:
        Communications();
        ~Communications();

        void initiate();

        void sendMessage(DataMessage& msg);

        bool getMessage(DataMessage& msg);

    private:

        std::unique_ptr<ClientWrapper> clientWrapper_;
};

#endif // COMMUNICATIONS_HH



