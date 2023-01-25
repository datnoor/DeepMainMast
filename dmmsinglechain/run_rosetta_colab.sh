#!/bin/bash

ID=$1
MODS=$4


./h_cmd_rosettaCM_setup.sh $ID

./i_cmd_rosettaCM.sh $ID $MODS
