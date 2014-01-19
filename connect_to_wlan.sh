#!/bin/bash
if [ "$(id -u)" != "0" ]; then
	echo "You need to run this script using sudo privileges."
	exit 1
fi

echo -e "This script is used to make the necessary config file to enable Rasberry Pi automatically connect to WPA-PSK and WPA2-PSK protected wireless networks.\n"
echo "Unplug the wlan adapter and press any key to continue..."
read
echo "Enter wlan SSID: "
read ssid
echo "Enter wlan password: "
read password

#variables
path="/etc/network/interfaces"
path_backup="/etc/network/interfaces_BACKUP"
output="auto lo
iface lo inet loopback
iface eth0 inet dhcp

auto wlan0
allow-hotplug wlan0
iface wlan0 inet dhcp
\twpa-ssid \"$ssid\"
\twpa-psk \"$password\"
"

echo -e "\nThe following output:"
echo "----------OUTPUT----------"
printf "$output"
echo -e "------END OF OUTPUT-------"
echo "Has been written to file: $path."

#write backup file
if [ -f $path_backup ];
then
   echo -e "Backup file already exist.\n"
else
   cp $path $path_backup
   echo -e "Backup file has been written to: $path/interfaces_BACKUP\n"
fi

#write config file
printf "$output" > $path

echo -e "All changes completed.
Turn off the Raspberry Pi and plugin your wlan-adapter. Start the Pi (45s). It should now have an wlan ip-address. You can verify this using 'ifconfig' after you reestablish the SSH. If you unplug the wired network you need restart the Pi."