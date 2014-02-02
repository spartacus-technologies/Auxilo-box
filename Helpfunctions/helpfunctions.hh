#ifndef HELPFUNCTIONS_HH
#define HELPFUNCTIONS_HH

#include <string>

namespace Help
{
	std::string intToStr(int value);
	int strToInt(const std::string & str);
	float strToFloat(const std::string & str);
	// Returns date in format: YYYY.MM.DD-hh:mm:ss
    std::string getCurrentTime(); 

	// Runs shell command 
    // returns command output as a string or "ERROR" in case of error
	std::string runCommand(const std::string & cmd);

	//returns last lines. Return empty string if file is not found or it is empty.
	std::string readLastNLinesFromFile(std::string file, int lines = 1);

	// returns d1 - d2
	int differenceBetweenDatesInSec(std::string d1, std::string d2);

	//Dates must be in format: YYYY.MM.DD-hh:mm:ss
	//return true also if dates are same.
	bool compareDates (std::string isThisNewerThan, std::string This);
}


#endif // HELPFUNCTIONS_HH
