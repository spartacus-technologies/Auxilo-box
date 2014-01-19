#!/bin/bash

# Returns temperature sensor (DS18B20) value or ERROR in case of error
# Parameter is device Id

# path=/sys/bus/w1/devices/28-$1;
path=/sys/bus/w1/devices/$1;

# no such device
if [ ! -d $path ]; then
	echo "ERROR"
	exit 0
fi

# Temperature is written in file w1_slave
temp_file=$(cat $path/w1_slave);

# Find temp from file
temp=$(echo $temp_file | awk '{ print $NF }' | \
 awk '{split($0,a,"="); print a[2]}')

echo $temp
