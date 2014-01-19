#include "Switch.hh"
#include "wiringPi/wiringPi/wiringPi.h"

//DEBUG
#include <iostream>

#define TEST_SWITCH_PIN 1

std::string sensorWasLastSeenOn = "";
bool sensorWasLastTimeOn = false;


Switch::Switch()
{

}

Switch::~Switch()
{

}

static bool lock = false;
void Switch::changeSensorStatus(bool status, std::string sensorTime)
{
	while(lock){}
		lock = true;

	if(sensorTime != "")
	{
		sensorWasLastSeenOn = sensorTime;
	}
	sensorWasLastTimeOn = status;

	lock = false;
}

static void interruptHandler()
{
	Switch::changeSensorStatus(true, "00:00");

    //Debug tuloste
	std::cout << "PIR havaitsee liiketta" << std::endl;

    //Energenie control
    // Plugs* socket;
    // socket = new Plugs();
    // socket->socketOn("1");
    // delete socket;
}

Sensor::sensorData Switch::readSensorStatus()
{
	while(lock){}

	lock = true;

	Sensor::sensorData returnValue;

	returnValue.sensorID = "motion_detector";
	returnValue.isSuccessful = sensorWasLastTimeOn;
	returnValue.read_time = sensorWasLastSeenOn;

	lock = false;

	return returnValue;
}

//Starts a new thread for interrupt handling:
bool Switch::init()
{	  
	wiringPiSetup();
	wiringPiISR(TEST_SWITCH_PIN, INT_EDGE_RISING, &interruptHandler);
	return true;
}

//This does not really do anything as interrupts are handled in own thread:
Sensor::sensorData Switch::getData() const
{
	Sensor::sensorData returnValue = readSensorStatus();

	//reset the sensor
	changeSensorStatus(false);

	return returnValue;
}
