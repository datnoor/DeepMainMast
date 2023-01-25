#!/bin/bash

ID=$1
TIME=$2
CONTOUR=$3
if [ ! -z $4 ] 
then 
    HETERO=$4
else
    HETERO="0"
fi
MODS=$5

bash 0_setup.sh $ID

bash a_cmd_emap2secplus.sh $ID $CONTOUR

bash b_cmd_node_long.sh $ID

bash c_cmd_path.sh $ID $TIME

bash d_cmd_modeling.sh $ID $TIME

bash g_cmd_combine_itr.sh $ID $TIME

if [[ $HETERO == 1 ]]; then

    echo "Processing Hetero oligomer pipeline"
    bash h_cmd_rosettaCM_setup.sh $ID
    bash i_cmd_rosettaCM.sh $ID $MODS

else

    echo "Processing Homo oligomer pipeline"
    bash g1_cmd_chain_assign_setup.sh $ID $TIME
    bash g2_cmd_chain_assign.sh $ID $TIME
    bash h1_cmd_rosettaCM_setup_rechain.sh $ID
    bash i1_cmd_rosettaCM_all.sh $ID $MODS

fi

bash j_cmd_daq_cm.sh $ID

bash k_cmd_dotscore.sh $ID

if [[ $HETERO == 1 ]]; then
    python l_ranker.py $ID $MODS
else
    python l1_ranker.py $ID $MODS
fi
