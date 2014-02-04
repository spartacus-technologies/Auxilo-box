#ifndef DEVICE_HH
#define DEVICE_HH

#include "../Communications/Protocol.pb.h"

class Device
{
public:
    virtual ~Device(){}

    void initialize() = 0;

    void setStatus(auxilo::DeviceState status) = 0;
    auxilo::DeviceState getStatus() const = 0;

    std::string getID() = 0;

};

#endif // DEVICE_HH