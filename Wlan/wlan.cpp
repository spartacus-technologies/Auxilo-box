#include "wlan.hh"
#include "../Helpfunctions/helpfunctions.hh"
// #include "helpfunctions.hh"


using namespace std;

namespace Wlan
{
	
vector<string> scan()
{
	string ssids_str = Help::runCommand("./scan.sh");

	vector<string> ssids;

	size_t pos = 0;
	string ssid;

	string del = "\n";

	while ( ( pos = ssids_str.find(del) ) != string::npos) 
	{
    	ssid = ssids_str.substr(0, pos);

    	ssids.push_back(ssid);

    	ssids_str.erase(0, pos + del.length());
	}

	return ssids;

}

bool connect(const string & SSID, const string & password)
{
	if (SSID.empty() or password.empty())
	{
		return false;
	}

	string output = Help::runCommand("./connect.sh SSID password");

	//TODO: Test if wlan0 is in use
	
	//...

	return true;
}

}