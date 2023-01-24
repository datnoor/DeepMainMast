#!/bin/bash

ID=$1
TIME=$2
CNT=$3
MODS=$4

./0_setup.sh $ID

./a_cmd_emap2secplus.sh $ID $CNT

./b_cmd_node_long.sh $ID

./c_cmd_path.sh $ID $TIME

./d_cmd_modeling.sh $ID $TIME

./g_cmd_combine_itr.sh $ID $TIME

./h_cmd_rosettaCM_setup.sh $ID

./i_cmd_rosettaCM.sh $ID $MODS

./j_cmd_daq_cm.sh $ID

./k_cmd_dotscore.sh $ID

python l_ranker.py $ID $MODS