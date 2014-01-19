#include "GPIO.hh"

#include <string>
#include <fstream>

#include "../Helpfunctions/helpfunctions.hh"

using namespace Help;

namespace GPIO
{
	std::string PATH_EXPORT = "/sys/class/gpio/export";
	std::string PATH_PIN = "/sys/class/gpio/gpio";

	bool setPinMode(int pin, PINMODE mode)
	{
		//Write pin number to file PATH_EXPORT in order to export it
		std::ofstream file (PATH_EXPORT.c_str());

		if ( file < 0)
		{
			return false;
		}

		file << pin;
		file.close();

		//Set mode by writing 'in' or 'out' to file path_direction
		std::string path_direction = PATH_PIN + intToStr(pin) +"/direction";
		file.open(path_direction.c_str());

		if ( file < 0)
		{
			return false;
		}

		if (mode == OUTPUT)
		{
			file << "out";
		}
		else
		{
			file << "in";
		}

		file.close();


		return true;
	}

	bool digitalWrite(int pin, int value)
	{
		// value can only be either 0 or 1, ie. low or high
		if (value != LOW and value != HIGH)
		{
			return false;
		}

		//Write value to file in path 'path_value'
		std::string path_value = PATH_PIN + intToStr(pin) +"/value";
		std::ofstream file (path_value.c_str());

		if ( file < 0)
		{
			return false;
		}

		file << intToStr(value);
		file.close();

		return true;
	}

	int digitalRead(int pin)
	{
		std::string path_value =  PATH_PIN + intToStr(pin) +"/value";
		std::ifstream file(path_value.c_str());
		
		if (file < 0)
		{
			return -1;
		}

		std::string value;
		file >> value;
		file.close();

		return strToInt(value);
	}

}
