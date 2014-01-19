#ifndef SENSOR_HH
#define SENSOR_HH

#include <string>

// Interface for sensors

class Sensor
{
	
public:
	
	//TODO: this struct as return value for getData():
	struct sensorData{
	
		float value;
		bool isSuccessful;
		std::string sensorID;
		std::string read_time;
	};
	
	virtual bool init() = 0;
	virtual sensorData getData() const = 0;

protected: 

private:
	
};

#endif // SENSOR_HH
