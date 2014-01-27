#!/bin/bash


wait_process()
{
	pid=$!

	running=$(ps -p $pid -o comm=)

	while [[ -n "$running" ]]; do
		running=$(ps -p $pid -o comm=)
		echo -n "."
		sleep 1
	done
}

# test if ran with sudo
if [ $USER != "root" ]; then
	echo "Please run this script as root. Aborting..."
	exit 1
fi

vlc_installed=$(apt-cache policy vlc | grep "none")
energ_installed=$(apt-cache policy sispmctl | grep "none")
boost_installed=$(apt-cache policy libboost-all-dev | grep "none")
protobuf_installed=$(apt-cache policy libprotobuf-dev | grep "none")
protobufcompiler_installed=$(apt-cache policy protobuf-compiler | grep "none")
cmake_installed=$(apt-cache policy cmake | grep "none")


# Install necessary programs and libraries
if [[ -n "$vlc_installed" || -n "$energ_installed" || -n "$boost_installed" || -n "$protobuf_installed" || -n "$protobufcompiler_installed" ]]; then

	# check if connected to internet
	echo "Checking internet connection..."
	connection=$(((ping -W 5 -c3 google.com) > /dev/null 2>&1) && echo "up" || (exit 1 && echo "down" ))
	if [ "$connection" != "up" ]; then
		echo "Please connect to internet. Aborting..."
		exit 1
	fi
	
	echo "Updating repositories"
	apt-get update > /dev/null 2>&1 &	
	
	wait_process
	echo "Done"

	# sispmctl is used for energenie
	if [[ -n "$energ_installed" ]]; then
		echo "Installing sispmctl for energenie"
		apt-get -y install sispmctl  > /dev/null 2>&1 &	
	
		wait_process

		energ_installed=$(apt-cache policy sispmctl | grep "none")

		if [[ -n "$energ_installed" ]]; then
			echo "Sispmctl installation failed!!"
			exit 1
		else
			echo "Done"
		fi

	fi

	# install vlc. Vlc is needed for sounds
	if [[ -n "$vlc_installed" ]]; then
		echo "Installing vlc"
		apt-get -y install vlc  > /dev/null 2>&1 &

		wait_process

		vlc_installed=$(apt-cache policy vlc | grep "none")
		if [[ -n "$vlc_installed" ]]; then
			echo "Vlc installation failed!!"
			exit 1
		else
			echo "Done"
		fi

	fi

	if [[ -n "$boost_installed" ]]; then
		echo "Installing boost"
		apt-get -y install libboost-all-dev  > /dev/null 2>&1 &	
	
		wait_process
		
		boost_installed=$(apt-cache policy libboost-all-dev | grep "none")
		if [[ -n "$boost_installed" ]]; then
			echo "Boost installation failed!!"
			exit 1
		else
			echo "Done"
		fi
	fi

	if [[ -n "$protobuf_installed" ]]; then
		echo "Installing protobuf"
		apt-get -y install libprotobuf-dev  > /dev/null 2>&1 &	
	
		wait_process

		protobuf_installed=$(apt-cache policy libprotobuf-dev | grep "none")
		if [[ -n "$protobuf_installed" ]]; then
			echo "Protobuf installation failed!!"
			exit 1
		else
			echo "Done"
		fi
	fi

	if [[ -n "$protobufcompiler_installed" ]]; then
		echo "Installing protobuf compiler"
		apt-get -y install protobuf-compiler  > /dev/null 2>&1 &	
	
		wait_process

		protobufcompiler_installed=$(apt-cache policy protobuf-compiler | grep "none")
		if [[ -n "$protobufcompiler_installed" ]]; then
			echo "Protobuf compiler installation failed!!"
			exit 1
		else
			echo "Done"
		fi
	fi
	
	if [[ -n "$cmake_installed" ]]; then
		echo "Installing cmake"  
		apt-get -y install cmake > /dev/null 2>&1 &	
	
		wait_process

		cmake_installed=$(apt-cache policy cmake | grep "none")
		if [[ -n "$cmake_installed" ]]; then
			echo "Cmake installation failed!!"
			exit 1
		else
			echo "Done"
		fi
	fi

fi



# Make scripts executable
echo "Making scripts executable..."
chmod u+x Sensors/Thermometer/Thermometer.pl
chmod u+x Devices/Speaker/repeat.sh
chmod u+x Devices/Speaker/stop.sh
chmod u+x Sensors/Switch/wiringPi/build
chmod u+x CLEAN.sh
chmod u+x Wlan/connect.sh
chmod u+x Wlan/scan.sh


# load necessary kernel modules in boot and now
echo "Loading kernel modules w1-gpio and w1-therm"

gpio_loaded=$(cat /etc/modules | grep "w1-gpio")

if [[ -z "$gpio_loaded" ]]; then
	echo w1-gpio >> /etc/modules
else	
	echo "w1-gpio already loaded"
fi

therm_loaded=$(cat /etc/modules | grep "w1-therm")

if [[ -z "$therm_loaded" ]]; then
	echo w1-therm >> /etc/modules
else	
	echo "w1-therm already loaded"
fi

modprobe w1-gpio && modprobe w1-therm

#Install wirinPi library for Switch
echo "Installing wiringPi"

cd Sensors/Switch/wiringPi/ 

./build > output.txt 2>&1 &

wait_process

echo "Done"

errors=$(echo -e "$(cat output.txt)" | grep "Make Failed")

if [[ -n "$errors" ]]; then
	echo "wiringPi installation failed!"
	echo -e "\n \nContact devteam."
	exit 1
fi

rm output.txt

cd ../../.. 

#Run CMake and make
echo "Making software"

#generate protobuf c++ classes
cd Communications/
protoc --cpp_out=. protocol.proto
cd ../

cmake CMakeLists.txt > output.txt 2>&1 &

wait_process

errors=$(echo -e "$(cat output.txt)" | grep "Error")


if [[ -n "$errors" ]]; then
	echo -e "\n \nThere were errors: "
	echo "$errors"
	echo -e "\nMore details in file \"output.txt\""
	echo -e "\n \nContact devteam."
	exit 1
fi

make > output.txt 2>&1 &

wait_process

errors=$(grep 'Error\|error\|Fatal' output.txt)

if [[ -n "$errors" ]]; then
	echo -e "\n \n There were errors: "
	echo "$errors"
	echo -e "\nMore details in file \"output.txt\""
	echo -e "\n \nContact devteam."
	exit 1
fi

echo "Done"

rm output.txt

echo -e "\n\nExecutables are in subfolder \"bin\""

mkdir log
exit 0
