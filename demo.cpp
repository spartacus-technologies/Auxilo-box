#include <string>
#include <iostream>
#include <unistd.h> //sleep
#include <vector>


#include "Devices/NexaPlug/NexaPlug.hh"
#include "Sensors/Switch/Switch.hh"
#include "Sensors/Thermometer/Thermometer.hh"
#include "Sensors/sensor.hh"
//#include "Communications/Communications.hh"

using namespace std;

bool initSensors(vector<Sensor*>* sensors);

int main(int argc, char const *argv[])
{
	NexaPlug::initRF();
	NexaPlug* nexa = new NexaPlug (1);

	vector<Sensor*> sensors; 
	initSensors(&sensors);

	//Communications* comm = new Communications();
	//comm->initiate();

	while (true)
	{

		sleep(1);

		//Check sensor data
		for(unsigned i = 0; i < sensors.size(); ++i)
		{
			Sensor::sensorData data = sensors.at(i)->getData();

			//Näiden dynamic_castien tilalle tulee aikanaan sitten niitä sääntöluokan olioita.
			//Tai sitten ei.
			//-------------------------------------------------------------------------------

			Switch* pir = dynamic_cast<Switch*>( sensors.at(i) );
			if ( pir != 0 and data.isSuccessful )
			{
				if (nexa->isSocketOn())
				{
					nexa->socketOff();
				}
				else
				{
					nexa->socketOn();
				}
			}

			Thermometer* therm = dynamic_cast<Thermometer*>( sensors.at(i) );
			if (therm != 0 and data.isSuccessful)
			{
				if (data.isSuccessful)
				{
					cout << "Lampotila on " << data.value << " astetta." << endl;
				}
				else
				{
					cout << "Lampotilan tarkastaminen epaonnistui! :("<< endl;
				}
			}

			//-------------------------------------------------------------------------------

		}

		//Read new messages
		//Datamessage msg();
		// while ( comm->getMessage(msg) )
		// {
		// 	   ....
		// }

		

	}

	return 0;
}

//function initializes all sensors and adds them to vector:
bool initSensors(vector<Sensor*>* sensors)
{
	
	Sensor* sensorPtr;
	
	//Thermometers:
	sensorPtr = new Thermometer();
	
	//Create new Thermoeters in loop until all have been assigned to objects.
	//Also, last object must be deleted since it has no hardware and thus is useless:
	while(sensorPtr->init())
	{
		//init OK -> append to vector:
		sensors->push_back(sensorPtr);
		//new sensor is created for next loop:
		sensorPtr = new Thermometer();
	}
	
	//latest Thermometer init was a failure -> delete object:
	delete sensorPtr;
	
	//Humidity sensor(s):
	
	// sensorPtr = new Humidity();
	
	//This cannot fail: (as far as I know...)
	// sensorPtr->init();

	//PIR-sensor
	sensorPtr = new Switch();
	sensorPtr->init();

	sensors->push_back(sensorPtr);

	//Some other sensor:
	//sensorPtr = new Switch();
	//...

	
	//Debug:
	cout<<"initSensors(): SensorVector size: "<<sensors->size()<<endl;
	
	//all went well:
	return true;
}