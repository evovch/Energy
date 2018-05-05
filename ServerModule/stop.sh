#!/bin/bash

ISRUNNING=`pgrep client_real.exe`
# echo ${ISRUNNING}

if [[ ${ISRUNNING} != "" ]]; then
        kill -s INT ${ISRUNNING}
	echo "STOPPED"
else
    	echo "NOT RUNNING"
fi

