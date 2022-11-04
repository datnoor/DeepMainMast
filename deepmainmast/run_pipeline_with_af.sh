#!/bin/bash

ID=$1

./0_setup.sh $ID

./a_cmd_emap2secplus.sh $ID

./b_cmd_node_long.sh $ID

./c_cmd_path.sh $ID

./d_cmd_modeling.sh $ID

./e_cmd_vesper_setup.sh $ID

./f_cmd_vesper.sh $ID

./g_cmd_combine_itr.sh $ID

./h_cmd_rosettaCM_setup.sh $ID

./i_cmd_rosettaCM.sh $ID

./j_cmd_daq_cm.sh $ID

./k_cmd_dotscore.sh $ID