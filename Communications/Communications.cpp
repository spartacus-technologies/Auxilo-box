#include <iostream>
#include <fstream>

#include "Communications.hh"
#include "ClientWrapper.hpp"

Communications::Communications()
{
    clientWrapper_.reset(new ClientWrapper(SERVERIPADDRESS, PORT));
}

Communications::~Communications()
{

}

bool Communications::initiate(auxilo::DeviceList list)
{
    // Lets save client identifiers locally first. Later when we get more
    // things going on the server we can implement web UI for customers to set
    // these directly there.
    std::filebuf configRead;
    if (!configRead.open ("config.cfg", std::ios::in)) {
        std::cerr << "Error while opening the config file." << std::endl;
        return false;
    }
    std::istream is(&configRead);
    std::string customerID;
    std::string deviceName;
    std::string boxClient;

    std::cout << "Reading configs from the config file...";
    if(!is.eof())
    {
        std::getline(is, customerID);
    }
    if(!is.eof())
    {
        std::getline(is, deviceName);
    }
    if(!is.eof())
    {
        std::getline(is, boxClient);
        if(boxClient.empty())
        {
            std::cerr << "Too few lines in the config file." << std::endl;
            return false;
        }
    }
    std::cout << " OK. Following configs read" << std::endl;
    std::cout << customerID << std::endl;
    std::cout << deviceName << std::endl;
    std::cout << boxClient << std::endl;

    bool isBox = false;
    if(!boxClient.compare("Box"))
    {
        isBox = true;
    }

    // Ask server to join customer room.
    if( !clientWrapper_->initiateConnection(customerID,deviceName,isBox))
    {
        std::cerr << "Client error: Server name not resolvable." << std::endl;
        return false;
    }

    // Send DeviceList
    clientWrapper_->sendDeviceList(list);

    // Handshake with the server should be OK.
    return true;
}

void Communications::sendMessage(auxilo::Message &msg)
{
    clientWrapper_->sendMessage(msg);
}

bool Communications::getMessage(auxilo::Message &msg)
{
    return clientWrapper_->getLastMessage(msg);
}

bool Communications::communicationStatus()
{
    return clientWrapper_->connectionStatus ();
}