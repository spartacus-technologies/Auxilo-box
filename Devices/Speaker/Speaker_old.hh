#ifndef SPEAKER_HH
#define SPEAKER_HH

#include <string>

class Speaker
{

public:
    
    Speaker();
    Speaker(const std::string & default_sound);
    ~Speaker();

    enum VLCStatus {STOPPED, PLAYING, PAUSED, NOT_RUNNING};
    VLCStatus getStatus();
    
    bool playSound(bool repeat=false, std::string path=DEFAULT);

    void setRepeatOff();
    void setRepeatOn();
    bool isRepeatOn();
    
    bool setDefaultSound(std::string path=DEFAULT);

    bool play();
    void pause();

private:
    std::string default_sound_;
    bool repeat_status_;

    static const std::string DEFAULT_SOUND;
    static const std::string DEFAULT;
};

#endif // SPEAKER_HH