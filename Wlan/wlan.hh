#ifndef WLAN_HH
#define WLAN_HH

#include <string>
#include <vector>

namespace Wlan
{
	std::vector<std::string> scan();
	bool connect(const std::string & SSID, const std::string & password);
}

#endif // WLAN_HH