#!/bin/bash

# arecord -q -t wav -d 5 audio.wav
arecord -q -D hw:1,0 -f s16_le -c 1 -d 15 > audio.wav

sox -q audio.wav -r 16000 -b 16 -c 1 audio.flac

language=$1
output="NO_OUTPUT"

if [[ "$language" == "US" ]]; then
	 output=$(wget -q -U "rate=16000" -O - "http://www.google.com/speech-api/v1/recognize?lang=en-US&client=Mozilla/5.0" --post-file audio.flac --header="Content-Type: audio/x-flac; rate=16000")
elif [[ "$language" == "FI" ]]; then
	 output=$(wget -q -U "rate=16000" -O - "http://www.google.com/speech-api/v1/recognize?lang=fi_FI&client=Mozilla/5.0" --post-file audio.flac --header="Content-Type: audio/x-flac; rate=16000")
else
	echo -n "ERROR: Use parameter US or FI"
	rm audio.wav audio.flac
	exit 0

fi

if [[ "$output" == "NO_OUTPUT" ]]; then
	echo -n "ERROR: Connection failed"
	rm audio.wav audio.flac
	exit 0
fi

result=$(echo $output | awk '{split($0,a,"\""); print a[12]}')
rm audio.wav audio.flac

echo -n "$result"

 # > google_output.json