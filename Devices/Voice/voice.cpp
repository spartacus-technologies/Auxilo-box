#include <iostream>
#include "voice.hh"
#include <string>
#include "../../Helpfunctions/helpfunctions.hh"
#include "../Speaker/Speaker.hh"

Voice::Voice()
{

}
Voice::~Voice()
{

}

Voice::VoiceCommand Voice::newVoiceCommand()
{
    //soita aloitusääni
    Speaker* speaker = new Speaker();
    speaker->playSound();

    //wget script
    std::string recorded_voice = Help::runCommand("Devices/Voice/voice.sh FI");

    std::cout << recorded_voice << "\n";
    
    //soita lopetusääni
    speaker->playSound();

    sleep(1);

    delete speaker;

    if(commands_[SPEAKER_PLAY] == recorded_voice)
    {
        return SPEAKER_PLAY;
    }
    else
    {
        return COMMAND_NOT_FOUND;
    }
}

void Voice::addNewCommand(VoiceCommand id, std::string google_word)
{
    commands_[id] = google_word;
}