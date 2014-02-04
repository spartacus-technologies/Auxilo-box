#ifndef NEXAPLUGS_HH
#define NEXAPLUGS_HH

#include <string>
//#include <map>
#include <inttypes.h>

#include "../Device.hh"

using namespace std;

class NexaPlug : public Device
{
public:
	void initialize();
	void setStatus(auxilo::deviceState status);
	auxilo::deviceState getStatus() const;
	std::string getID() const;
    
    NexaPlug(unsigned int id);
    ~NexaPlug();
    //bool init();

    void pair();
    void unpair();
    void socketOn();
    void socketOff();
	bool isSocketOn();

	static void initRF();
	
private:
	
	///map<string ID, bool> status;	//pistorasioiden ID:t & statukset (=viimeksi annettu käsky, ei välttämättä vastaa todellista tilannetta)

	// Parameters for nexaTxPacket
	uint32_t id_; 
	uint8_t state_;      // 0-1
	uint8_t channel_;    // 0-15 Mitähän lie tekee?
	uint8_t group_;      // 0-1 
	uint8_t dim_level_;  // 0-15 Mitähän lie tekee?
	uint8_t repeats_;
    
	static const uint8_t brand_;

	void sendPacket(uint8_t state, uint8_t  repeats = 5);

	auxilo::deviceState deviceStatus;
};

#endif // NEXAPLUGS_HH