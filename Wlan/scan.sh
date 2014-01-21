#!/bin/bash

# Scannaa wlanit ja palauttaa stringeinä wlanien nimet, erotinmerkki vaikka \n

# TODO: Tässä pitää ensin varmistaa, että interface wlan0 on olemassa.
# scan=$(iwlist wlan0 scan)

# scan=$(cat wlan_scan_fail.txt)
scan=$(cat wlan_scan.txt)

i=0
while [[ -n $(echo "$scan" | grep "No scan results") ]]; do
	
	if [[ $i -ge 5 ]]; then
		echo "Scan failed"
		exit 1
	fi

	# scan=$(iwlist wlan0 scan)
    ((++i))

done

ssids=$(echo "$scan" | grep "ESSID" | awk -F "[\"\"]" '{ for (i=2; i<NF; i+=2) print $i }') 

echo "$ssids"

exit 0