#!/bin/bash

SUFFIX=`date +day%j_time%H%M%S`
FILENAME=out_${SUFFIX}.txt
DATETIME=`date`

ISRUNNING=`pgrep client_real.exe`

if [[ ${ISRUNNING} != "" ]]; then

echo "ALREADY RUNNING, NOTHING DONE"	

else

cat > ${FILENAME} << EOF
Started on ${DATETIME}
EOF

nohup ./client_real.exe q 42001 q q >> ${FILENAME} 2>&1 &

echo "STARTED"

fi
