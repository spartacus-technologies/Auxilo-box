#!/bin/bash

# Scannaa wlanit ja palauttaa stringeinä wlanien nimet, erotinmerkki vaikka \n

# Test if there is wlan interface
interface=$(ifconfig | grep "wlan0")

if [[ -z "$interface" ]]; then
	echo "Scan failed: no wlan interface"
	exit 1
fi

interface=$(echo "$interface" | cut -c 1-5)

scan=$(iwlist $interface scan)

# scan=$(cat wlan_scan_fail.txt)
# scan=$(cat wlan_scan.txt)

# Trying to scan 5 times.
i=0
while [[ -n $(echo "$scan" | grep "No scan results") ]]; do
	
	if [[ $i -ge 5 ]]; then
		echo "Scan failed"
		exit 1
	fi
	sleep 0.5
	scan=$(iwlist $interface scan)
    ((++i))

done

# Parse SSIDs
ssids=$(echo "$scan" | grep "ESSID" | awk -F "[\"\"]" '{ for (i=2; i<NF; i+=2) print $i }') 

echo "$ssids"

exit 0