#include <iostream>
#include "Speaker.hh"
#include "../../Helpfunctions/helpfunctions.hh"

using namespace std;

const string Speaker::DEFAULT_SOUND = "Resources/Sounds/beep.mp3";
const string Speaker::DEFAULT = std::string("default");

// https://wiki.videolan.org/VLC_command-line_help
// http://www.raspberrypi.org/phpBB3/viewtopic.php?f=66&t=29403

Speaker::Speaker(): default_sound_(DEFAULT_SOUND), repeat_status_(false)
{
	//Set system volume to 100%
	Help::runCommand("amixer sset 'PCM' 100%");
}

Speaker::Speaker(const string & default_sound):
 default_sound_(default_sound), repeat_status_(false)
{
	//Set system volume to 100%
	Help::runCommand("amixer sset 'PCM' 100%");
}

Speaker::~Speaker(){}

//returnvalue according to sucessful sound playing
bool Speaker::playSound( bool repeat, string path)
{
	Stop();

	if (path == DEFAULT)
	{
		path = default_sound_;
	}

	if (repeat)
	{
		Help::runCommand("Devices/Speaker/repeat.sh "+path + " >/dev/null &");
		repeat_status_=true;
	}
	else
	{
		Help::runCommand("mplayer "+path +  " >/dev/null &");
		repeat_status_=false;
	}

	return true;

}  

void Speaker::Stop() 
{
	if (!repeat_status_)
	{
		string pid="";
		pid=Help::runCommand("pidof mplayer");

		if ( pid != "")
		{
			Help::runCommand("killall mplayer");
		}
	}
	else
	{
		Help::runCommand("speaker/stop.sh");
	}
	
}

