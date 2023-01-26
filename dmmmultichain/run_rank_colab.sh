#!/bin/bash

ID=$1
MODS=$2
HETERO=$3

./j_cmd_daq_cm.sh $ID

./k_cmd_dotscore.sh $ID

if [[ $HETERO == 1 ]]; then
    python l_ranker.py $ID $MODS
else
    python l1_ranker.py $ID $MODS
fi