#include <string>
#include <iostream>
#include <unistd.h> //sleep
#include <vector>


#include "Devices/NexaPlug/NexaPlug.hh"
#include "Sensors/Switch/Switch.hh"
#include "Sensors/Thermometer/Thermometer.hh"
#include "Sensors/sensor.hh"
#include "Communications/Communications.hh"

using namespace std;

bool initSensors(vector<Sensor*>* sensors);
const string BOXID = "1";

int main(int argc, char const *argv[])
{
	NexaPlug::initRF();
	NexaPlug* nexa = new NexaPlug (1);

	vector<Sensor*> sensors; 
	initSensors(&sensors);

    //// Communications

    // Create DeviceList to be sent to the server.
    auxilo::DeviceList listOfDevices;


    // Fill all entries to the newDevice (at least those which are required).
    for(unsigned i = 0; i < sensors.size(); ++i)
    {
    	// Add new device to the list.
    	auxilo::DeviceInfo* newDevice = listOfDevices.add_devicelist();

    	Switch* pir = dynamic_cast<Switch*>( sensors.at(i) );

    	if (pir != 0)
    	{
    		newDevice->set_type(auxilo::deviceType::sensorPIR);
    		cout << "Uusi pir-anturi" << endl;
    	}
    	else
    	{
    		Thermometer* therm = dynamic_cast<Thermometer*>( sensors.at(i) );

    		if ( therm != 0 )
    		{
    			newDevice->set_type(auxilo::deviceType::sensorTemp);
    			cout << "Uusi lampotila-anturi" << endl;
    		}
    		else //unknown sensortype (unknown to protocol)
    			continue;
    	}
    	

    	(*newDevice->mutable_boxid()) = BOXID;
    	// (*newDevice->mutable_description()) ="Kuvaus";
    	(*newDevice->mutable_deviceid()) = sensors.at(i)->getData().sensorID;
    	// (*newDevice->mutable_aliasname()) = "Human readable entry";
    }	


    //Lisätään 1 Nexa, koska laitteiden yhteistä rajapintaa ei ole tehty.
	auxilo::DeviceInfo* newDevice = listOfDevices.add_devicelist();
    

    newDevice->set_type(auxilo::deviceType::deviceNexa); // Setting temp sensor device.
    (*newDevice->mutable_boxid()) = BOXID;
    // (*newDevice->mutable_description()) ="Kuvaus";

    // DEVICES NEED IDs!!!
    (*newDevice->mutable_deviceid()) = "NEXA";
    // (*newDevice->mutable_aliasname()) = "Human readable entry";

    //When all devices are in the list, send it to server and complete the handshake.
    cout << "comm tehdään" << endl;
    Communications comm;
    cout << "comm init" << endl;
    comm.initiate(listOfDevices);
	cout << "comm valmis" << endl;
    //// Communications initialized.

	while (true)
	{

		sleep(10);

		//Check sensor data
		for(unsigned i = 0; i < sensors.size(); ++i)
		{
			Sensor::sensorData data = sensors.at(i)->getData();

			// //Näiden dynamic_castien tilalle tulee aikanaan sitten niitä sääntöluokan olioita.
			// //Tai sitten ei.
			// //-------------------------------------------------------------------------------

			// Switch* pir = dynamic_cast<Switch*>( sensors.at(i) );
			// if ( pir != 0 and data.isSuccessful )
			// {
			// 	if (nexa->isSocketOn())
			// 	{
			// 		nexa->socketOff();
			// 	}
			// 	else
			// 	{
			// 		nexa->socketOn();
			// 	}
			// }

			Thermometer* therm = dynamic_cast<Thermometer*>( sensors.at(i) );
			if (therm != 0 and data.isSuccessful)
			{
				if (data.isSuccessful)
				{
					//Tässä data talteen johonkin säiliöön?
					cout << "Lampotila on " << data.value << " astetta." << endl;

					//KOMMUNIKAATION TESTAUSTA VARTEN!
					//-------------------------------------
					auxilo::DataMessage datamsg;
				   	datamsg.set_hardwareid(data.sensorID);
				   	datamsg.set_data(data.value);
				   	datamsg.set_timestamp(data.read_time);
 
    				auxilo::Message ans;
	   				ans.set_devicename(BOXID);
	   				(*ans.mutable_datamesg()) = datamsg;

	   				comm.sendMessage(ans);
	   				cout << "message sent" << endl;
	   				//-------------------------------------
				}
				else
				{
					cout << "Lampotilan tarkastaminen epaonnistui! :("<< endl;
				}
			}

			//-------------------------------------------------------------------------------

		}

		//Read new messages
		cout << "check mailbox" << endl;
		auxilo::Message msg;
		while ( comm.getMessage(msg) )
		{
			cout << "new message!!!" << endl;
			  //Query for sensors
			  if ( msg.has_qry() and msg.qry().has_sensorid() )
			  {
			   		string sensorID = msg.qry().sensorid();

			   		//TÄMÄ TIETO JOSTAIN SÄILIÖSTÄ!
			   		for(unsigned i = 0; i < sensors.size(); ++i)
			   		{
			   			Sensor::sensorData data = sensors.at(i)->getData();

			   			if ( data.sensorID == sensorID )
			   			{
			   				cout << "Sending sensordata: " << sensorID << ": " << data.value << endl;

			   				//Create message...
			   				auxilo::DataMessage datamsg;
			   				datamsg.set_hardwareid(sensorID);
			   				datamsg.set_data(data.value);
			   				datamsg.set_timestamp(data.read_time);

			   				auxilo::Message ans;
			   				ans.set_devicename(BOXID);
			   				(*ans.mutable_datamesg()) = datamsg;

			   				//... and send it.
			   				comm.sendMessage(ans);

			   			}
			   		}
			  }
		}

		

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