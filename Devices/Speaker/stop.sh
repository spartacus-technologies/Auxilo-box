#!/bin/bash


line_count=$(ps -ef | grep "repeat.sh" | wc -l)
pid=$(ps -ef | grep "repeat.sh")

if [[ $line_count -eq 2 ]]; then

	pid=$(echo $pid | awk '{split($0,a," "); print a[2]}')
	kill -9 $pid

	mplayer=$(pidof mplayer)

	if [[ -n "$mplayer" ]]; then
		killall mplayer
	fi
	
fi