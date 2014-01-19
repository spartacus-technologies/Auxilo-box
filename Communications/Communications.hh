#ifndef COMMUNICATIONS_HH
#define COMMUNICATIONS_HH

#include <string>

const std::string SERVERIPADDRESS = "xx.xx.xx.xx";
const std::string AuthenticationID = "Auxilo123123";

class Communications
{

public:
	
	Communications();	//Empty?
	~Communications();
	
	bool initConnection(); 	//Connection to server
	
};

#endif // COMMUNICATIONS_HH
