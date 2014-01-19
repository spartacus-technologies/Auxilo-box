#ifndef HUMIDITY_HH
#define HUMIDITY_HH

#include <string>
#include "../sensor.hh"

const float ERROR_HUM = -1;

class Humidity: public Sensor
{
    public:
    
    Humidity();
    ~Humidity();
	
    bool init();
	virtual sensorData getData() const;
};

#endif // HUMIDITY_HH