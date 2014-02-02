#!/bin/bash

# returns time between dates in seconds. Argument format: YYYY.MM.DD-hh:mm:ss
# For example:
# 		./date_diff_sec.sh 2014.02.02-14:51:00 2014.02.02-14:50:58 
#  		
# 		output = 2

# -Vesa Naumanen

arg1="$1"
arg2="$2"

day1=$(echo "$arg1" | awk '{split($0,a,"-"); print a[1]}')
tim1=$(echo "$arg1" | awk '{split($0,a,"-"); print a[2]}')

day2=$(echo "$arg2" | awk '{split($0,a,"-"); print a[1]}')
tim2=$(echo "$arg2" | awk '{split($0,a,"-"); print a[2]}')

# echo "day = ${day//[.]/} \n time = $tim"

D1=$(date +%s -d "${day1//[.]/} $tim1")
D2=$(date +%s -d "${day2//[.]/} $tim2")

((diff_sec=D1-D2))

echo $diff_sec

exit 0