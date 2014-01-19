#!/bin/bash

running=$(pidof vlc)

# vlc running
if [[ -n "$running" ]]; then
	
	status=$(dbus-send --print-reply --dest=org.mpris.MediaPlayer2.vlc /org/mpris/MediaPlayer2 \
	 org.freedesktop.DBus.Properties.Get string:'org.mpris.MediaPlayer2.Player' string:'PlaybackStatus'| \
	 awk '{split($0,a,"\""); print a[2]}')

	status=$(echo $status | cut -c1-)

	echo -n "$status"

	exit 0

fi

echo -n "Not running"
exit 0
