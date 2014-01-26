#ifndef CLIENTOBSERVER_HPP
#define CLIENTOBSERVER_HPP

#include "protocol.pb.h"

// This class can be used to implement the observer functionality to
// client side application code, which uses this network 'library'.
// In general, the observer is notified after network-client receives
// messages from the server, calling the update-method.

// If application code utilizes observer functionality, it needs
// to extend this ClientObserver class and implement the pure
// virtual update-method. Additionally application code need to initialize
// observer functionality by informing the ClientWrapper-class that it has
// an active observer. This can be done by calling ClientWrapper::addObserver(this)
// in the application code.
// ExampleClient-class gives an example, how to use this observer-functionality.


class DataMessage;
class ClientWrapper;

class ClientObserver
{
    public:

        ClientObserver();

        void addSubject(ClientWrapper* ptr);

        virtual ~ClientObserver();

        virtual void update(auxilo::Message& msg) = 0;

    private:
        ClientWrapper* wrapper_;
};

#endif // CLIENTOBSERVER_HPP
