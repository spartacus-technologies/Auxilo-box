#ifndef SWITCH_HH
#define SWITCH_HH

#include "../sensor.hh"

class Switch: public Sensor
{
public:
    Switch();
    ~Switch();

    virtual bool init();
    virtual sensorData getData() const;
	static void changeSensorStatus(bool status, std::string sensorTime="");

private:
	static Sensor::sensorData readSensorStatus();
};

#endif // SWITCH_HH
