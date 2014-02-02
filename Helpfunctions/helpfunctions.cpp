#include "helpfunctions.hh"

#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <time.h>

namespace Help
{
	std::string intToStr(int value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	int strToInt(const std::string & str)
	{
		int num = 0;
		std::istringstream iss(str);
		iss >> num;
		return num;
	}
	
	float strToFloat(const std::string & str){
	
		float num = 0;
		std::istringstream iss(str);
		iss >> num;
		return num;
	}
	
	// Runs shell command 
    // returns command output as a string  or "ERROR" in case of error
	std::string runCommand(const std::string & cmd)
	{
		// popen paramaters char* command, char* type,
		// type is either read or write
		FILE* pipe = popen(cmd.c_str(), "r");

		if(!pipe)
		{
			return "ERROR";
		}

		std::string output = "";
		char buffer[128];
		//feof tests the end-of-file indicator for the stream
		while( !feof(pipe) )
		{
			// fgets() reads in at most one less than size characters from stream
			// and stores them into the buffer
			if(fgets(buffer, 128, pipe) != NULL)
			{
				output += buffer;
			}
    		
		}

		pclose(pipe);
    	return output;

	}

	std::string getCurrentTime()
	{
		return runCommand("date  +\"%Y.%m.%d-%R:%S\" | tr -d \"\n\"");

	    // time_t rawtime;
	    // struct tm * timeinfo;

	    // time(&rawtime);
	    // timeinfo = localtime(&rawtime);
	    // std::string time_temp = asctime(timeinfo);
	    // return time_temp.substr(0,time_temp.size()-1);
	}

	std::string readLastNLinesFromFile(std::string file, int lines)
	{
		std::string cmd = "tail -"+intToStr(lines)+" "+file;
		std::string output = runCommand(cmd);

		std::string error = runCommand("echo \""+output+"\" | grep \"No such file\"");

		if ( !error.empty() )
		{
			return "";
		}

		return output;
	}

	int differenceBetweenDatesInSec(std::string d1, std::string d2)
	{
		return strToInt( runCommand("Helpfunctions/date_diff_sec.sh " + d1 + " " + d2) );
	}

	bool compareDates (std::string isThisNewerThan, std::string This)
	{
		if (differenceBetweenDatesInSec( isThisNewerThan, This ) >= 0)
			return true;

		return false;
	}
}
