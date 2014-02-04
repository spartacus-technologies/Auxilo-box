#include <string>
#include <iostream>
#include "Devices/NexaPlug/NexaPlug.hh"
#include "Sensors/Switch/Switch.hh"

using namespace std;

int main(int argc, char const *argv[])
{
	NexaPlug* nexa = new NexaPlug (1);
	Switch* pir = 0;
	string input;

	while (true)
	{
		cout << "$: ";
		getline (cin, input);

		cout << input << endl;

		if (input == "nexa 1")
		{
			nexa->socketOn();
			cout << "Nexa päälle" << endl;
		}
		else if ( input == "nexa 0" )
		{
			nexa->socketOff();
			cout << "Nexa kiinni" << endl;	
		}
		if (input == "pir 1")
		{
			if (pir == 0)
			{
				pir = new Switch();
				pir->init();
			}

			cout << "Liiketunnistin päälle" << endl;
		}
		else if ( input == "pir 0" )
		{
			if (pir != 0)
			{
				delete pir;
			}

			cout << "Liiketunnistin kiinni" << endl;	
		}
		else if ( input == "q" ) break;
	}
	return 0;
}