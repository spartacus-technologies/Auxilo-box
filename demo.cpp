#include <string>
#include <iostream>
#include "Devices/NexaPlug/NexaPlug.hh"
#include "Sensors/Switch/Switch.hh"
#include "Sensors/Thermometer/Thermometer.hh"
#include "Sensors/sensor.hh"
#include <unistd.h> //sleep

using namespace std;

int main(int argc, char const *argv[])
{
	NexaPlug::initRF();
	NexaPlug* nexa = new NexaPlug (1);

	Sensor* thermometer = new Thermometer();
	thermometer->init();

	Sensor* pir = new Switch();
	pir->init();

	while (true)
	{
		sleep(1);

		if (pir->getData().isSuccessful)
		{
			if (nexa->isSocketOn())
			{
				nexa->socketOff();
			}
			else
				nexa->socketOn();
		}

		Sensor::sensorData temp = thermometer->getData();

		if (temp.isSuccessful)
		{
			cout << "Lampotila on " << temp.value << " astetta." << endl;
		}
		else
		{
			cout << "Lampotilan tarkastaminen epaonnistui! :("<< endl;
		}

	}

	return 0;
}