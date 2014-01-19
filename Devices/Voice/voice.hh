#ifndef VOICE_HH
#define VOICE_HH

#include <string>
#include <map>

class Voice
{
public:
    Voice();
    ~Voice();


    enum VoiceCommand
    {
        SPEAKER_PLAY,
        SPEAKER_STOP,
        COMMAND_NOT_FOUND
    };

    VoiceCommand newVoiceCommand();


    void addNewCommand(VoiceCommand id, std::string google_word);

private:
    std::map<VoiceCommand, std::string> commands_;
};

#endif