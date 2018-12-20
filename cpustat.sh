#!/bin/bash
CUR_DIR=$(pwd)
echo $CUR_DIR
printf "Cpustat starting..\n"
printf "Recording the procfile..\n<please ctrl+c to kill>\n"
printf "The result can be accessed on \n"
printf $CUR_DIR
printf "\n Processing...\n"
while true; do
	./cpustat.out  >> $CUR_DIR/cpustat_$(date +"%y%m%d_%H").txt
  sleep 1
 done
