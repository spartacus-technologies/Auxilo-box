#include <string>
#include <iostream>
#include <unistd.h> //sleep
#include <vector>
#include <fstream>
#include <map>

#include "Devices/NexaPlug/NexaPlug.hh"
#include "Sensors/Switch/Switch.hh"
#include "Sensors/Thermometer/Thermometer.hh"
#include "Sensors/sensor.hh"
#include "Communications/Communications.hh"
#include "Helpfunctions/helpfunctions.hh"

using namespace std;


bool initSensors(vector<Sensor*>* sensors);
void writeLog(const string& sensor_id, const string& date, float value);
string getDateFromLogLine(const string& line);
float getValueFromLogLine(const string& line);
void initDevices(map<string, Device*> &devices);
bool getBoxId();

static string BOXID = "";
const string LOGDIR = "log/";
const string LOGFILETYPE = ".csv";

const int MAIN_LOOP_DELAY = 1;
const int THERM_CHECK_DELAY = 60*15/MAIN_LOOP_DELAY; //in seconds


int main(int argc, char const *argv[])
{
	if(!getBoxId())
	{
		exit(1);
	}

	cout << BOXID << "\n";

	vector<Sensor*> sensors;
	map<string, Device*> devices;
	initSensors(&sensors);
	initDevices(devices);

    //// Initialize Communications
	//------------------------------------------------------
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
    		cout << "New PIR added to deviceList" << endl;
    	}
    	else
    	{
    		Thermometer* therm = dynamic_cast<Thermometer*>( sensors.at(i) );

    		if ( therm != 0 )
    		{
    			newDevice->set_type(auxilo::deviceType::sensorTemp);
    			cout << "New thermometer added to deviceList" << endl;
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
    (*newDevice->mutable_deviceid()) = "nexa 1"; //FIXME
    // (*newDevice->mutable_aliasname()) = "Human readable entry";

    //When all devices are in the list, send it to server and complete the handshake.
    Communications* comm = new Communications();
    comm->initiate(listOfDevices);
    // Communications initialized.
    //------------------------------------------------------

	int therm_check = THERM_CHECK_DELAY;
	bool therm_checked = false;

	//**********************
	//		MAIN LOOP 
	//**********************
	while (true)
	{

		sleep(MAIN_LOOP_DELAY);


		//Check sensor data
		for(unsigned i = 0; i < sensors.size(); ++i)
		{
			Sensor::sensorData data = sensors.at(i)->getData();

			// //Näiden dynamic_castien tilalle tulee aikanaan sitten niitä sääntöluokan olioita.
			// //Tai sitten ei.
			// //-------------------------------------------------------------------------------

			Switch* pir = dynamic_cast<Switch*>( sensors.at(i) );
			if ( pir != 0 and data.isSuccessful )
			{
				// if (nexa->isSocketOn())
				// {
				// 	nexa->socketOff();
				// }
				// else
				// {
				// 	nexa->socketOn();
				// }

				cout << data.read_time <<"   PIR detected movement" << endl;
				writeLog(data.sensorID, data.read_time, 1);
				continue;
			}

			Thermometer* therm = dynamic_cast<Thermometer*>( sensors.at(i) );
			if (therm != 0 and therm_check >= THERM_CHECK_DELAY)
			{
				if (data.isSuccessful)
				{
					//Data logiin talteen
					cout << data.read_time << "    Thermometer " << data.sensorID  << "  :   " << data.value << "C" << endl;

					//KOMMUNIKAATION TESTAUSTA VARTEN!
					//-------------------------------------
					/*
					auxilo::DataMessage datamsg;
				   	datamsg.set_hardwareid(data.sensorID);
				   	datamsg.set_data(data.value);
				   	datamsg.set_timestamp(data.read_time);
 
    				auxilo::Message ans;
	   				ans.set_devicename(BOXID);
	   				(*ans.mutable_datamesg()) = datamsg;

	   				comm.sendMessage(ans);
	   				cout << "message sent" << endl;
	   				*/
	   				//-------------------------------------

	   				writeLog(data.sensorID, data.read_time, data.value);
	   				therm_checked = true;
	   				continue;

				}
				else
				{
					cout << "Therm check failed!"<< endl;
				}
			}
		}

		//Reset thermometertimer.
		++therm_check;
		if ( therm_checked )
		{
			therm_check = 0;
			therm_checked = false;
		}
		

		//Check if connection to server is lost
		if ( !comm->communicationStatus() )
		{
			cout << "Connection lost" << endl;
			delete comm;
			comm = new Communications();
			
			if (comm->initiate(listOfDevices)) 
				continue;
		}

		//Read new messages recieved from the server.
		auxilo::Message msg;
		while ( comm->getMessage(msg) )
		{
			  cout << "New message recieved!" << endl;
			  auxilo::Message ans;
			  ans.set_senderdevicename(BOXID);
			  ans.set_receiverdevicename(msg.senderdevicename());

			  //Query for sensors
			  if ( msg.has_qry() )
			  {
			  		auxilo::QueryMessageList qml = msg.qry();

			  		cout << "Sensor query count: " << qml.query_size() << endl;

			  		//List of data of all the required senosors to be send to the server
			  		auxilo::SensDataList sensordatalist;

			  		//Process all queries
			  		for ( int i = 0; i < qml.query_size(); ++i )
			  		{
			  			auxilo::QueryMessage qry = qml.query(i);

			  			string sensorID = qry.sensorid();

			  			// cout << "SENSORID = " << sensorID << endl;

			  			vector<std::string> sensor_values;

			  			// line_count to be read from the logfile
			  			int line_count = 1;

			  			if ( qry.has_latestdate() )
			  			{
			  				//TODO: PIRRIÄ EI VIELÄ OTETA HUOMIOON
			  				int possible_count = Help::differenceBetweenDatesInSec( Help::getCurrentTime(), qry.latestdate() ) / THERM_CHECK_DELAY;

			  				if (possible_count >= 1)
			  				{
			  					line_count += possible_count;
			  				}
			  			}

				   		string results = Help::readLastNLinesFromFile(LOGDIR + sensorID+LOGFILETYPE, line_count);
				   		
				   		// cout << "Linecount = " << line_count << endl;
				   		// cout << "Sensor: " << sensorID << "   value = " << results << endl;

				   		//Parse results and push them to the sensordatalist
				   		while ( results.length() > 0 )
				   		{
				   			// cout << "OLEN WHILESSÄ" << endl;
				   			//Get one line from string
				   			unsigned delimeter_pos = results.find("\n");

							if ( delimeter_pos < 1 ) break;

							string result = results.substr(0, delimeter_pos);

							results.erase(0, delimeter_pos + 1);

							
				   			string date = getDateFromLogLine(result);

				   			// if date in log is older than date in query, ingnore it.
				   			if ( qry.has_latestdate() and Help::compareDates( qry.latestdate(), date ))
				   				continue;

							float value = getValueFromLogLine(result);	
							auxilo::DataMessage* datamsg = sensordatalist.add_sensordata();
				   			datamsg->set_hardwareid(sensorID);
				   			datamsg->set_data(value);
				   			datamsg->set_timestamp(date);
				   			cout << "Sensor:" << sensorID  << "    value:    " << value << endl;
				   		}

			  		}

			  		( *ans.mutable_sensordatalist() ) = sensordatalist;
                    cout << "SensorDataList size " << sensordatalist.sensordata_size() << endl;

			  }
			  if ( msg.has_device_command() )
			  {
			  		cout << "Device cmd" << endl;
			  		auxilo::DeviceCommand cmd = msg.device_command();
			  		string devID = cmd.deviceid();

			  		auxilo::DeviceStatus ds;
			  		ds.set_deviceid(devID);

			  		cout << "DeviceID " <<  devID << endl;

			  		//Find pointer to the device
			  		std::map<std::string, Device*>::iterator devItr = devices.find(devID);
			  		
			  		//Device not found, should not happen
			  		if(devItr == devices.end())
			  		{
			  			std::cerr << "Device not found" << std::endl; //FIXME
			  		}

			  		if (cmd.has_setstatus())
			  		{
			  			switch (cmd.setstatus())
			  			{
			  			case auxilo::deviceState::on:
							  cout << "Set device on" << endl;
							  (*devItr).second->setStatus(auxilo::deviceState::on);
							  break;
						case auxilo::deviceState::off:
							  cout << "Set device off" << endl;
							  (*devItr).second->setStatus(auxilo::deviceState::off);
							  break;
						default:
							  cout << "Unknown status" << endl;
							  break;
			  			}
			  		}

			  		ds.set_status((*devItr).second->getStatus());

			  		( *ans.mutable_device_status() ) = ds;
			  }

			  //Send the answer
			  comm->sendMessage(ans);
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
	
	//Debug:
	cout<<"initSensors(): SensorVector size: "<<sensors->size()<<endl;
	
	//all went well:
	return true;
}

void writeLog(const string& sensor_id, const string& date, float value)
{
	ofstream file;

	file.open(LOGDIR + sensor_id+LOGFILETYPE, ios::out | ios::app);

	if (file.is_open())
  	{
	    file << date+",";
	    file << value;
	    file << "\n";
	    file.close();
  	}
}

string getDateFromLogLine(const string& line)
{
	unsigned delimeter_pos = line.find(",");

	if ( delimeter_pos < 1 ) return "";

	return line.substr(0, delimeter_pos);
}

float getValueFromLogLine(const string& line)
{
	unsigned delimeter_pos = line.find(",");

	if ( delimeter_pos < 1 ) return 0;

	return Help::strToFloat(line.substr(delimeter_pos+1));

}

void initDevices(map<string, Device*> &devices)
{
	Device* nexa = new NexaPlug (1);
	cout << "NEXAN ID = " << nexa->getID() << endl;
	nexa->initialize();
	devices.insert(pair<string, Device*>(nexa->getID(), nexa));
}

bool getBoxId()
{
	string tempFile = Help::readLastNLinesFromFile("config.cfg",100);

	if(tempFile.empty())
	{
		cerr << "config.cfg is empty or not exist.\n";
		return false;
	}
	for(int i=0; i<2;++i)
	{
	   	//Get one line from string
		unsigned delimeter_pos = tempFile.find("\n");

		if ( delimeter_pos < 1 ) break;

		string result = tempFile.substr(0, delimeter_pos);

		BOXID = result;

		tempFile.erase(0, delimeter_pos + 1);
	}
	return true;
}