#include "Humidity.hh"

#include "../../Helpfunctions/helpfunctions.hh"

#include <iostream>

#include <unistd.h> //sleep

using namespace std; //cin, cout, string, vector

Humidity::Humidity()
{
	//Nothing to do here really...
}

Humidity::~Humidity()
{
	//Same here...
}

bool Humidity::init(){



}

Sensor::sensorData Humidity::getData() const{

	string temp_str = Help::runCommand("/home/pi/Desktop/Software/Sensors/Humidity/DHT22.pl hum");
	
	Sensor::sensorData returnValue;
	
	while ( temp_str == "WRONG_VALUE" or temp_str == "ERROR")
	{	
		int i = 0;
		//Tries 3 times to get the temperature value
		if ( temp_str == "ERROR" or i > 2)
		{	
			cout << "ERROR" << endl;
			cout<<"Some serious error"<<endl;
			returnValue.value = ERROR_HUM;
			returnValue.isSuccessful = false;
			returnValue.sensorID = "(null)";
			return returnValue;
		}

		sleep(1);
		++i;
		temp_str = Help::runCommand("/home/pi/Desktop/Software/Sensors/Humidity/DHT22.pl hum");
	}
	
	returnValue.value = Help::strToFloat(temp_str);
	returnValue.isSuccessful = true;
	returnValue.sensorID = "(null)";
	returnValue.read_time = Help::getCurrentTime();
	return returnValue;
	


}