#ifndef THERMOMETER_HH
#define THERMOMETER_HH

#include <string>
#include <vector>

#include "../sensor.hh"

//Example device:
// /sys/bus/w1/devices/28-000004d9c71c/w1_slave

//Note: kaikilla 1-Wire laitteilla on yksilöllinen ID -numero
const float ERROR_TEMP = -300.0;

class Thermometer: public Sensor
{
public:
    Thermometer();
    ~Thermometer();

    virtual bool init();
    virtual sensorData getData() const;

private:
		
	static const std::string sensorPrefix;//Indicates that sensor is Thermometer
	static const std::string sensorPath;
	
	std::string sensorID_;	//unique ID for this object
	
	//List of already reserved thermometers:
	static std::vector<std::string> IDList;
	
};

#endif // Thermometer_HH

