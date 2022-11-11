#!/bin/bash

ID=$1
TIME=$2

./b_cmd_node_long.sh $ID

./c_cmd_path.sh $ID $TIME

./d_cmd_modeling.sh $ID $TIME