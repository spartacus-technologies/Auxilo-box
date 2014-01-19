#include <iostream>
#include "Speaker.hh"
#include "../../Helpfunctions/helpfunctions.hh"

using namespace std;

const string Speaker::DEFAULT_SOUND = "Resources/Sounds/beep.mp3";
const string Speaker::DEFAULT = std::string("default");

// https://wiki.videolan.org/VLC_command-line_help

Speaker::Speaker(): default_sound_(DEFAULT_SOUND), repeat_status_(false)
{
	//Set system volume to 100%
	Help::runCommand("amixer sset 'Master' 100%");
}

Speaker::Speaker(const string & default_sound):
 default_sound_(default_sound), repeat_status_(false)
{
	//Set system volume to 100%
	Help::runCommand("amixer sset 'Master' 100%");
}

Speaker::~Speaker(){}

bool Speaker::playSound( bool repeat, string path)
{
	if (path == DEFAULT)
	{
		path = default_sound_;
	}

	if (getStatus() == NOT_RUNNING)
	{
		if (repeat and !isRepeatOn())
		{
			path+=" --repeat";
			repeat_status_ = true;
		}
		else if( !repeat and isRepeatOn() )
		{
			path+= " --no-repeat";
			repeat_status_ = false;
		}
	}
	else
	{
		pause();

		if (repeat)
		{
			setRepeatOn();
		}
		else
		{
			setRepeatOff();
		}
	}


	//Run vlc without interface in background
	Help::runCommand("cvlc "+path+" --control dbus --one-instance >/dev/null &");

//ei liene vlc vielä auki kun näitä suoritetaan...

	// if ( getStatus() != PLAYING )
	// {
	// 	return false;
	// }

	return true;

}    

void Speaker::setRepeatOn()
{
	if (isRepeatOn())
	{
		return;
	}

	Help::runCommand("qdbus org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Set org.mpris.MediaPlayer2.Player LoopStatus Track");

	repeat_status_ = true;
}

void Speaker::setRepeatOff()
{
	if (!isRepeatOn())
	{
		return;
	}
	
	Help::runCommand("qdbus org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 org.freedesktop.DBus.Properties.Set org.mpris.MediaPlayer2.Player LoopStatus None >/dev/null");

	repeat_status_ = false;
}
    
bool Speaker::isRepeatOn()
{
	return repeat_status_;
}

bool Speaker::play()
{
	VLCStatus status = getStatus();
	if ( status != PAUSED)
	{
		if (status == PLAYING)
		{
			return true;
		}

		return false;
	}

	Help::runCommand("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.PlayPause >/dev/null");

	return true;
}

void Speaker::pause()
{
	if (getStatus() != PLAYING)
	{
		return;
	}

	Help::runCommand("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.PlayPause >/dev/null");

}


bool Speaker::setDefaultSound(string path)
{
	//Tarkistuksia että kyseinen kipale on olemassa
	default_sound_=path;
}

Speaker::VLCStatus Speaker::getStatus()
{
	string status=Help::runCommand("Devices/Speaker/vlcStatus.sh");

	if (status == "Playing")
	{
		return PLAYING;
	}
	if (status == "Paused")
	{
		return PAUSED;
	}
	if (status == "Stopped")
	{
		return STOPPED;
	}

	return NOT_RUNNING;
}
