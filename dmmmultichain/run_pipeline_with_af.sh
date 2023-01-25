#!/bin/bash

ID=$1
TIME=$2
CNT=$3
if [ "$4" -eq "1" ] 
then 
    HETERO="1"
else
    HETERO="0"
fi
MODS=$5

./0_setup.sh $ID

./a_cmd_emap2secplus.sh $ID $CNT

./b_cmd_node_long.sh $ID &> /results/output1.log &
./b_cmd_node_long.sh $ID &> /results/output2.log &
./b_cmd_node_long.sh $ID &> /results/output3.log &
./b_cmd_node_long.sh $ID &> /results/output4.log &
wait

./c_cmd_path.sh $ID $TIME &> /results/output1.log &
./c_cmd_path.sh $ID $TIME &> /results/output2.log &
./c_cmd_path.sh $ID $TIME &> /results/output3.log &
./c_cmd_path.sh $ID $TIME &> /results/output4.log &
wait

./d_cmd_modeling.sh $ID $TIME &> /results/output1.log &
./d_cmd_modeling.sh $ID $TIME &> /results/output2.log &
./d_cmd_modeling.sh $ID $TIME &> /results/output3.log &
./d_cmd_modeling.sh $ID $TIME &> /results/output4.log &
wait

./e_cmd_vesper_setup.sh $ID

./f_cmd_vesper.sh $ID

./g_cmd_combine_itr.sh $ID $TIME

if [[ $HETERO == 1 ]]; then
    echo "Processing Hetero oligomer pipeline"
    ./h_cmd_rosettaCM_setup.sh $ID
    ./i_cmd_rosettaCM.sh $ID $MODS
else
    echo "Processing Homo oligomer pipeline"
    ./g1_cmd_chain_assign_setup.sh $ID $TIME
    ./g2_cmd_chain_assign.sh $ID $TIME
    ./h1_cmd_rosettaCM_setup_rechain.sh $ID
    ./i1_cmd_rosettaCM_all.sh $ID $MODS
fi

./j_cmd_daq_cm.sh $ID

./k_cmd_dotscore.sh $ID

if [[ $HETERO == 1 ]]; then
    python l_ranker.py $ID $MODS
else
    python l1_ranker.py $ID $MODS
fi
