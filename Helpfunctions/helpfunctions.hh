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

	//returns last lines. Return empty string if file is not found or it is empty.
	std::string readLastNLinesFromFile(std::string file, int lines = 1);
}


#endif // HELPFUNCTIONS_HH
