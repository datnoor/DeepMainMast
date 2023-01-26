#!/bin/bash

ID=$1
TIME=$2
CNT=$3
USE_AF=$4

./0_setup.sh $ID &>> /content/output

./a_cmd_emap2secplus.sh $ID $CNT &>> /content/output

./b_cmd_node_long.sh $ID &> /content/output1.log &
./b_cmd_node_long.sh $ID &> /content/output2.log &
wait

./c_cmd_path.sh $ID $TIME &> /content/output1.log &
./c_cmd_path.sh $ID $TIME &> /content/output2.log &
wait

./d_cmd_modeling.sh $ID $TIME &> /content/output1.log &
./d_cmd_modeling.sh $ID $TIME &> /content/output2.log &
wait

if [[ "$USE_AF" -eq 1 ]]; then
    ./e_cmd_vesper_setup.sh $ID &>> /content/output

    ./f_cmd_vesper.sh $ID &>> /content/output
fi

./g_cmd_combine_itr.sh $ID $TIME &>> /content/output
