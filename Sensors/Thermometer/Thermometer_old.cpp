#include "Thermometer.hh"
#include "../../Helpfunctions/helpfunctions.hh"

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>     /* atoi */

using namespace std; //cin, cout, string, vector

//const -values:
const string Thermometer::sensorPrefix = "28";	//Indicates that sensor is Thermometer
const string Thermometer::sensorPath = "/sys/bus/w1/devices/";
vector<string> Thermometer::IDList;

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
	cout<<"Thermometer::init()"<<endl;
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
		cout<<"Thermometer::init(): new device added: "<<sensorID_<<endl;
		IDList.push_back(temp);
		return true;
		
		
		//Jump point from inner loop:
		nextDevice:;
		
		//inner loop ends: device was already known -> move to next device
	}
	
	
	cout<<"Thermometer::init(): no new devices found"<<endl;
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

//TODO skriptissä jotain bugia. En saanut toimimaan. Polun/nimen antamisessa on jotain vikaa? -Eetu

Sensor::sensorData Thermometer::getData() const
{
	Sensor::sensorData returnValue;
	//runCommand returns output of a skript as a string and 1000 times too big.
	//Value is in Celcius. Returns "ERROR" if sensorID is wrong
	//cout<<"/sys/bus/w1/devices/" + sensorID_<<endl;
	string temp_str = Help::runCommand("/home/pi/Desktop/Software/Sensors/Thermometer/Thermometer.sh "+ sensorID_);

	if ( temp_str == "ERROR" )
	{	
		cout<<"Some serious error"<<endl;
		returnValue.value = ERROR_TEMP;
		returnValue.isSuccessful = false;
		returnValue.sensorID = sensorID_;
		return returnValue;
	}
	
	returnValue.value = Help::strToInt (temp_str)/ 1000.0;;
	returnValue.isSuccessful = true;
	returnValue.sensorID = sensorID_;
	return returnValue;
}

// ^^ Olin kans jotain skriptiä aluks miettimässä, mutta C++'ssan Tiedosto IO'lla homma onnistui myös:
// -Eetu
// PS tuli vähän kahteen kertaan tehtyä hommaa, kun en huomannut tekemääsi skriptiä.

// Aiempaan viitaten muuten tuossa skriptissä pitänee huomioida anturin yksilöllinen ID, jos halutaan
// erottaa vaikkapa makuuhuoneen lämpötila olohuoneen lämpötilasta.

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
