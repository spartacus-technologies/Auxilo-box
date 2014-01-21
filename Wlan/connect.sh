#!/bin/bash

#Parametrinä ssid ja salasana, joka sitten luo sen tiedoston (kts. connect_to_wlan.sh)

path="/etc/network/interfaces"
path_backup="/etc/network/interfaces_BACKUP"

ssid=$1
password=$2

output="auto lo
iface lo inet loopback
iface eth0 inet dhcp

auto wlan0
allow-hotplug wlan0
iface wlan0 inet dhcp
\twpa-ssid \"$ssid\"
\twpa-psk \"$password\"
"

#write backup file
if [ -f $path_backup ];
then
   echo -e "Backup file already exist."
else
   cp $path $path_backup
   echo -e "Backup file has been written to: $path/interfaces_BACKUP"
fi

#write config file
printf "$output" > $path

exit 0