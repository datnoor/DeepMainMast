#!/bin/bash

ID=$1
TIME=$2
MODS=$3
if [ "$4" -eq "1" ] 
then 
    HETERO="1"
else
    HETERO="0"
fi

if [[ $HETERO == 1 ]]; then
    echo "Processing Hetero oligomer pipeline"
    ./h_cmd_rosettaCM_setup.sh $ID
    ./i_cmd_rosettaCM.sh $ID $MODS
else
    echo "Processing Homo oligomer pipeline"
    ./h1_cmd_rosettaCM_setup_rechain.sh $ID
    ./i1_cmd_rosettaCM_all.sh $ID $MODS
fi
