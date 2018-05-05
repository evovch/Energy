#!/bin/bash

ISRUNNING=`pgrep client_real.exe`
# echo ${ISRUNNING}

if [[ ${ISRUNNING} != "" ]]; then
	echo "RUNNING"
else
	echo "NOT RUNNING"
fi
