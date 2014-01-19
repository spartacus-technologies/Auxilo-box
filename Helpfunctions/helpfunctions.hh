#ifndef HELPFUNCTIONS_HH
#define HELPFUNCTIONS_HH

#include <string>

namespace Help
{
	std::string intToStr(int value);
	int strToInt(const std::string & str);
	float strToFloat(const std::string & str);
    std::string getCurrentTime();

	// Runs shell command 
    // returns command output as a string or "ERROR" in case of error
	std::string runCommand(const std::string & cmd);
}


#endif // HELPFUNCTIONS_HH
