# DeepMainMast

This repository contains the code implementation of "paper name and link". There are two folders i) dmmsinglechain supportd single chain protein maps ii) dmmmultichain supports multi chain protein maps.

## Dependencies

Use the requirements.txt file and install packages using pip as follows. There are same requirements for both single chain and multi-chain.

```
pip install -r requirements.txt
```

Then install BioTEMPy package using pip. Installing it separately is crucial because it has dependecy conflicts with bio package. Hence after installing all packages
using requirements.txt, we then separately install BioTEMPy using pip as follows

```
pip install BioTEMPy
```

## How to run

First choose which pipeline you want to execute, Single chain or Multi-chain. Follow the details for each pipeline. 

## DeepMainMast for Single Chain protein maps

Put your inputs in "data" folder in deepmainmast directory.

The input folder must include the following. Optionally required inputs are flagged optional

1) input.map - The cryo-EM map .map file [optional if input.mrc is present]

2) input.mrc - The cryo-EM map .mrc file [optional if input.map is present]

3) seq.fasta - Sequence fasta file associated with the input map

4) afmodel - folder containing the output of alphafold model for this input map. Please have a look at deepmainmast/input/2n1fA/afmodel in this repo to get a better idea



For eg, for 2n1fA, the input folder should follow the following directory structure -

```
deepmainmast
    |
    |________ input
                |
                |________  2n1fA
                            |
                            |________   afmodel
                            |
                            |________   input.map
                            |
                            |________   input.mrc
                            |
                            |________   seq.fasta
```

You can also go and look at these actual paths at deepmainmast/input/2n1fA ... in this repository to get better idea.

To add your new input, just make a new folder under deepmainmast/input/ and name it with your input map id. For eg - deepmainmast/input/[ my_input_map_id ]


## Run DeepMainMast

cd (change directory) into deepmainmast folder and execute the following commands sequentially one after another. Replace 2n1fA with your input map id name (name of the folder you created under deepmainmast/input folder).

```
./0_setup.sh 2n1fA

./a_cmd_emap2secplus.sh 2n1fA

./b_cmd_node_long.sh 2n1fA

./c_cmd_path.sh 2n1fA

./d_cmd_modeling.sh 2n1fA

./e_cmd_vesper_setup.sh 2n1fA

./f_cmd_vesper.sh 2n1fA

./g_cmd_combine_itr.sh 2n1fA

./h_cmd_rosettaCM_setup.sh 2n1fA

./i_cmd_rosettaCM.sh 2n1fA

./j_cmd_daq_cm.sh 2n1fA

./k_cmd_dotscore.sh 2n1fA
```
