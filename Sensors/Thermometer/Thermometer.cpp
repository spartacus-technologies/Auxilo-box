#include "Thermometer.hh"
#include "../../Helpfunctions/helpfunctions.hh"

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>     /* atoi */

#include <unistd.h> //sleep

using namespace std;
//const -values:
const string Thermometer::sensorPrefix = "28";	//Indicates that sensor is Thermometer
const string Thermometer::sensorPath = "/sys/bus/w1/devices/";
std::vector<std::string> Thermometer::IDList;

Thermometer::Thermometer()
{
	//Nothing to do here really...
}

Thermometer::~Thermometer()
{
	//Same here...
}

bool Thermometer::init()
{
	std::cout<<"Thermometer::init()"<<std::endl;
	stringstream deviceIDs;
	deviceIDs << Help::runCommand("find " + sensorPath + "28*");
	
	
	string temp;	//deviceID for inspection
	
	//get all lines in avalable devices:
	while(getline(deviceIDs, temp)){
		
		
		//compare to all known devices found in vector:
		int index = 0;
		while(index != IDList.size()){
		
			//Device is already known -> jump to next device:
			if(temp == IDList.at(index)){
			
				goto nextDevice;
			}
			++index;
		}
		
		//list ended and device was not known:
		sensorID_ = temp.substr(temp.find(sensorPrefix));
		//sensorID_ = "28-000004d97167";
		std::cout<<"Thermometer::init(): new device added: "<<sensorID_<<std::endl;
		IDList.push_back(temp);
		return true;
		
		
		//Jump point from inner loop:
		nextDevice:;
		
		//inner loop ends: device was already known -> move to next device
	}
	
	
	std::cout<<"Thermometer::init(): no new devices found"<<std::endl;
	//outer loop ends: all devices were already known
	return false;
	
	//TODO:
	
	//tarkistukset
	// Joko 	modprobe -l <module name> 
	// tai 		lsmod (listaa kaikki ladatut kernel moduulit)
	//       -> lsmod | grep -o <module name> 


	//ei toteutusta? vai tarkistukset, että kernelmoduulit on ladattu?
	
	//^^jos moduulien latauksen tarkastus onnnistuu, niin mikä ettei.
	//Muussa tapauksessa ainakin testiarvon lukeminen lienee paikallaan. -Eetu
	
	//Ehkä jopa kernelmoduulien lataus, jos eivät vielä ladattuna
	
	//Lisä-huom: noita antureita tulee olemaan samassa väylässä useita, joten
	//olisiko parasta luoda yksi Themometer -olio jokaista anturia kohden?
	//Siinä tapauksessa pitäisi tehdä joku säiliö johonkin, jotta samalle anturille ei luoda 
	//kahta oliota. Anturit on yksilöity ID -numeroilla (kts. getData()).
}

Sensor::sensorData Thermometer::getData() const
{
	Sensor::sensorData returnValue;
	//runCommand returns output of a skript as a string and 1000 times too big.
	//Value is in Celcius. Returns "ERROR" if sensorID is wrong
	//cout<<"/sys/bus/w1/devices/" + sensorID_<<endl;
	string temp_str = Help::runCommand("Sensors/Thermometer/Thermometer.pl "+ sensorID_);

	int i=0;
	while ( temp_str == "WRONG_VALUE" or temp_str == "ERROR")
	{
		//Tries 3 times to get the temperature value
		if ( temp_str == "ERROR" or i > 2)
		{	
			std::cout << "ERROR" << std::endl;
			std::cout<<"Some serious error"<<std::endl;
			returnValue.value = ERROR_TEMP;
			returnValue.isSuccessful = false;
			returnValue.sensorID = sensorID_;
			return returnValue;
		}

		sleep(1);
		++i;
		temp_str = Help::runCommand("Sensors/Thermometer/Thermometer.pl "+ sensorID_);
	}
	
	returnValue.value = Help::strToInt (temp_str)/ 1000.0;;
	returnValue.isSuccessful = true;
	returnValue.sensorID = sensorID_;
	returnValue.read_time = Help::getCurrentTime();
	
	return returnValue;
}


/*
Sensor::sensorData Thermometer::getData() const
{		

		Sensor::sensorData returnValue;
		
		ifstream myReadFile;
		
		//cout<<sensorPath + sensorID_<<endl;
		
		try{
		
			myReadFile.open((sensorPath + sensorID_ + "/w1_slave").c_str());
		}catch(...){
		
			cout<<"Some serious error"<<endl;
			returnValue.value = ERROR_TEMP;
			returnValue.isSuccessful = false;
			returnValue.sensorID = sensorID_;
			
			return returnValue;
		}
		
		string output;
		
		if (myReadFile.is_open()) {

			while (!myReadFile.eof()) {

				myReadFile >> output;
			}
		}
		else{
			
			cout<<"Some serious error"<<endl;
			returnValue.value = ERROR_TEMP;
			returnValue.isSuccessful = false;
			returnValue.sensorID = sensorID_;
			
			return returnValue;
		}
		
		string ptr = output.substr(output.find("t=") + 2);
		float temp = atoi(ptr.c_str())/1000.0;
		
			returnValue.value = temp;
			returnValue.isSuccessful = true;
			returnValue.sensorID = sensorID_;
		
		myReadFile.close();
		
		return returnValue;
}
*/
//Tässä esimerkki suoraan w1_slave -tiedoston sisällöstä:
/*
pi@raspberrypi /sys/bus/w1/devices/28-000004d9c71c $ cat w1_slave
e0 01 4b 46 7f ff 10 10 27 : crc=27 YES
e0 01 4b 46 7f ff 10 10 27 t=26125
*/
