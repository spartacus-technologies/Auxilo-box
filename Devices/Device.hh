#ifndef DEVICE_HH
#define DEVICE_HH

#include "../Communications/Protocol.pb.h"

class Device
{
public:
    virtual ~Device(){}

    virtual void initialize() = 0;

    virtual void setStatus(auxilo::deviceState status) = 0;
    virtual auxilo::deviceState getStatus() const = 0;

    virtual std::string getID() const = 0;

};

#endif // DEVICE_HH