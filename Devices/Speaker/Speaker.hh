#ifndef SPEAKER_HH
#define SPEAKER_HH

#include <string>

class Speaker
{

public:
	
	Speaker();
    Speaker(const std::string & default_sound);
	~Speaker();

	//bool init();
    
	//returnvalue according to sucessful sound playing
	bool playSound(bool repeat=false, std::string path=DEFAULT);
    void Stop();

private:
    std::string default_sound_;
    bool repeat_status_;

    static const std::string DEFAULT_SOUND;
    static const std::string DEFAULT;
};

#endif // SPEAKER_HH