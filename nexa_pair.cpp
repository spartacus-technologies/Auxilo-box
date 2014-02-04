#include <iostream>
#include <string>
#include "Sensors/sensor.hh"
#include "Devices/NexaPlug/NexaPlug.hh"

using namespace std;

int main(int argc, char const *argv[])
{

	int id = 1;
	if(argc == 2)
	{
		id = (int)argv[1];
	}

	NexaPlug* nexa = new NexaPlug (id);

	//Unpair nexa
	while(true)
	{
		std::cout << "Plug in your nexa device and press ENTER within 3 seconds";
		std::string input;
		getline(cin,input);

		std::cout << "Wait..." << std::endl;

		nexa->unpair();

		std::cout << "Did you hear mechanical clicking sound (from the relay)? (y/n) ";
		getline(cin,input);

		if(input == "y")
		{
			break;
		}
		else
		{
			std::cout << "Try again? (y/n) " << std::endl;
			getline(cin,input);
			if(input == "n")
			{
				return 0;
			}
			std::cout << "Unplug the nexa device from the wall socket." << std::endl;
		}
	}

	//Pair nexa
	while(true)
	{
		std::cout << "Unplug the nexa device from the wall socket." << std::endl;
		std::cout << "Plug in your nexa device and press ENTER within 3 seconds";
		std::string input;
		getline(cin,input);

		std::cout << "Wait..." << std::endl;

		nexa->pair();

		std::cout << "Did you hear mechanical clicking sound (from the relay)? (y/n) ";
		getline(cin,input);
		if(input == "y")
		{
			break;
		}
		else
		{
			std::cout << "Try again? (y/n) ";
			getline(cin,input);
			if(input == "n")
			{
				return 0;
			}
		}
	}

	delete nexa;
	std::cout << "All done. Have a nice day!" << std::endl;

	return 0;
}