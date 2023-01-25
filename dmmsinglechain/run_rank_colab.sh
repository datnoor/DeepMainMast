#!/bin/bash

ID=$1
MODS=$4

./j_cmd_daq_cm.sh $ID

./k_cmd_dotscore.sh $ID

python l_ranker.py $ID $MODS