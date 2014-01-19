//RaspberryPi -softa
// Eetu & Vesa

#include <iostream>
#include <vector>

#include "Sensors/sensor.hh"
#include "Sensors/Switch/Switch.hh"
#include "Sensors/Thermometer/Thermometer.hh"
#include "Sensors/Humidity/Humidity.hh"

#include "Communications/Communications.hh"

#include "Devices/Plugs/Plugs.hh"
#include "Devices/Voice/voice.hh"
#include "Devices/Speaker/Speaker.hh"

using namespace std;

bool initSensors(vector<Sensor*>*);
bool initDevices();


int main()
{	
	
	//for debug:
	//initDevices();




	// //Objects:
	// Communications serverconnection;
	// serverconnection.initConnection();
		
	vector<Sensor*> sensors; 
	
	initSensors(&sensors);
	
	// //Energenie control
	// Plugs* socket;
	// socket = new Plugs();

	// //Motion detector control
	// Switch* motion_detector;
	// motion_detector = new Switch();
	// motion_detector->init();
	
	//main -loop:
	while(1){
	
		//cout << "loop..." << endl;
			
		//Read Values:
			
		//Test:
		for(unsigned i = 0; i < sensors.size(); ++i){
			
			Sensor::sensorData data = sensors.at(i)->getData();
			
			//Debug:
			cout<<"Sensor -ID: "<<data.sensorID<<endl;
			cout<<"Read OK: "<<data.isSuccessful<<endl;
			cout<<"Value: "<<data.value<<endl;
			cout<<"Time: "<<data.read_time<<endl;
		}
			
		//Send data to server:
		// 	//...
		
		//Sleep some time or wait for server trigger:
		sleep(2);

	// 	// socket->socketOff("1");
	// 	// sleep(2);
	// 	// socket->socketOn("1");
	}

	return 0;
}

bool initDevices(){

	//Speaker & Voice:
	
	Speaker* speaker = new Speaker();
	Voice* voice = new Voice();
	voice->addNewCommand(Voice::SPEAKER_PLAY, "soita");
	Voice::VoiceCommand cmd = voice->newVoiceCommand();
	delete voice;

	if(cmd == Voice::SPEAKER_PLAY)
	{
		speaker->playSound(false, "Resources/Sounds/NukeAlarm.mp3");
	}
	else
	{
		speaker->playSound();
	}

	delete speaker;
	
	return true;
}

//function initializes all sensors and adds them to vector:
bool initSensors(vector<Sensor*>* sensors){
	
	Sensor* sensorPtr;
	
	//Thermometers:
	sensorPtr = new Thermometer();
	
	//Create new Thermoeters in loop until all have been assigned to objects.
	//Also, last object must be deleted since it has no hardware and thus is useless:
	while(sensorPtr->init()){
		
		//init OK -> append to vector:
		sensors->push_back(sensorPtr);
		//new sensor is created for next loop:
		sensorPtr = new Thermometer();
	}
	
	//latest Thermometer init was a failure -> delete object:
	delete sensorPtr;
	
	//Humidity sensor(s):
	
	sensorPtr = new Humidity();
	
	//This cannot fail: (as far as I know...)
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
