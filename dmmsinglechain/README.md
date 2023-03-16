### Running DeepMainMast Single-chain on your EM map
#### 1. Input your map
1.1 Go in the data directory in this folder.

1.2 Make a new folder and name it anything you want (for eg - you can name the folder as the name of your input map). This name will serve as job name and will be an input to DeepMainMast pipeline.

1.3 Now put the input files in this newly created folder (input.map and/or input.mrc, seq.fasta and optional af2_model,pdb) just like in our example folders (for eg. refer folder 6sper).

Note: If you want to get an Alphafold model for you map then there are multiple ways to do it. One of the easiest way is to use https://github.com/sokrypton/ColabFold.

#### 2. Configure parameters for running DeepMainMast

You must control the execution of DeepMainMast pipeline using 5 variables in run.sh => ID, TIME, CNT, use_AF, MODELS. To do this open "run.sh" file.

Refer the following details to set each parameter under Configuration parameters section. 

a) ID => Specify the name of your job/map in ID variable. It is the folder name of your map files.

Example - In the data folder there are several maps (one folder for every map), and if you want to run map 2xeaA in the folder data/2xeaA then assign the folder name to ID variable as <b>ID=2xeaA</b>. If you created your own folder and uploaded files to it, then to run this folder set the ID variable as <b>ID="name of your folder"</b>

b) TIME => Some steps in DeepMainMast take a lot of time to compute. You can specify the number of seconds to be used per iteration for these compute heavy steps using TIME variable. If you keep value TIME to be very low then sometimes DeepMainMast might not work correctly. It is recommended to start with TIME=90 for your new map. But even after setting it to 90 gets you an error or DeepMainMast does not generate resulting files then it probably means that DeepMainMast needs more time to process the map and you need to increase TIME. Everytime your execution fails try adding 30 seconds. For eg if 90 doesn't work then try 90+30=120, then 120+30=150 etc. Note that you only have limited amount of code ocean quota. You will not be able to run very large maps on code ocean due to this limitation. 

Example - If you want to use 120 seconds per iteration then update the TIME variable as <b>TIME=120</b>

c) CNT => Specify the recommended contour level for your map in contour_level variable. 

Example - If the recommended contour level of the map is 2.5 then update the contour level variable as : <b>CNT=2.5</b>

d) use_AF => Specify the whether or not to use the Alpha Fold model. All of our examples have Alphafold model output .pdb file. If a map does not have an AlphFold model then you can still run the DeepMainMast pipeline. However, there will be less number of resulting .pdb files generated in /results/ranked folder as DeepMainMast will not run algorithms that use Alphafold model.

Example - If the Alpha Fold model .pdb file exists in the input folder then set the use_AF varaiable as 1 and 0 otherwise : <b>use_AF=1</b></pre>

e) MODELS => Specify the number of final models (.pdb files) to be generated in results/ranked folder. There are 4 algorithms (DM only, AF only, all, Vesper) which is given as input to the Rosetta step that generates several models for each of the algorithm. If you increase the value of MODELS variable then your topmost resulting .pdb files in ranked folder (i.e. rank1.pdb) might be more accurate and vice versa. However, Rosetta takes a lot of time for computation and is the most compute heavy step in the whole DeepMainMast pipeline. See the last section of this file for more details.

Example - If you specify <b>MODELS=1</b> then Rosetta will generate 1 model per algorithm i.e. you will get total 4 .pdb files in ranked folder. If you specify <b>MODELS=5</b> then Rosetta will generate 5 models per algorithm i.e. you will get total 20 .pdb files in ranked folder

#### 3. Run DeepMainMast Multi-chain
Run the multi-chain pipeline using the following command

<pre>
./run.sh
</pre>

or 

<pre>
bash run.sh
</pre>

#### 4. Get Output of DeepMainMast 
Your results will be generated under results/\<your job name\> directory. Under this directory you will find directory called ranked (results/\<your job name\>/ranked), which has all the models ranked according to their DAQ and DOT scores. The ranks are given based on the higher score that is Rank 1 model has the highest score.

The ranked folder .pdb files are actually the .pdb files ["S_singletgt_0001.pdb", "S_singletgt_0002.pdb", "S_singletgt_0003.pdb", "S_singletgt_0004.pdb", "S_singletgt_0005.pdb"] from these 4 folders ["CM_all", "CM_VESPER", "CM_DMonly", "CM_AFonly"] under the results/\<your job name\>/ranked folder. 

#### 5. Output Files
The DeepMainmast protocol generates the following output files in results/\<your job name\> directory.
#### Examples of output files (https://kiharalab.org/emsuites/deepmainmast/examples/)
- Single chain target 3j9sA (https://kiharalab.org/emsuites/deepmainmast/examples/3j9sA.zip)

##### Emap2sf output (Predicting local propaties of the protein structure): unet/
###### Atom type prediction:
<pre>
atom_BG.mrc		Background
atom_C.mrc		Backbone Carbon
atom_CA.mrc		Calpha
atom_CB.mrc		Cbeta
atom_N.mrc		Backbone Nitrogen
atom_O.mrc		Backbone Oxgen
atom_Others.mrc	Other atoms, Side-chain Atoms
</pre>

###### Amino Acid Type Prediction: unet/
<pre>
sigmoidAA_XXX.mrc 20 amino acid type
</pre>
##### Predicted Local Dense Points:
<pre>
NODE_p0.3.pdb	NODE_p0.4.pdb	NODE_p0.5.pdb
</pre>
##### Computed Paths using VRP Solver:
<pre>
PATH_p*Nch*.csv
</pre>
##### Computed Fragment Library:
<pre>
INP_p*Nch*Nali*.txt
</pre>
##### Computed Fragment Library with the AF2 model (if provided):
<pre>
INP_p*Nch*Nali*R*.txt
</pre>
##### Assembled fragments in PDB format:
<pre>
For each fragment library (INP*.txt), DeepMainmast generates one output (OUT*.pdb)
OUT_p*Nch*Nali*.pdb
</pre>
##### Map-model fitting results using VESPER (if AF2 model is provided): VESPER_MODELs/
<pre>
af2_A_R*.out				output file of VESPER computation
R*_A_FIT_MODEL*.pdb		Fitted models
</pre>

##### Input files for Assembling C-alpha Models:
###### Input1: concatenated models
<pre>
MODELs_AFonly.pdb		Concatenated all OUT*.pdb files
MODELs_DMonly.pdb		Concatenated OUT*.pdb files without the AF2 data
MODELs_VESPER.pdb		Concatenated fitted models in VESPER_MODELs/
MODELs_all.pdb			Concatenated all OUT*.pdb files
</pre>

###### Input2: matrix files
<pre>
MTX_AFonly.txt
MTX_DMonly.txt
MTX_VESPER.txt
MTX_all.txt
</pre>
##### Output: Assembled Calpha Models
<pre>
COMBINEi_AFonly.pdb
COMBINEi_DMonly.pdb
COMBINEi_VESPER.pdb
COMBINEi_all.pdb
</pre>

##### Ranked C-alpha Models using DAQ and DOT scores:
<pre>
pre_rosetta_ranked/
rank1.pdb
rank2.pdb
rank3.pdb
rank4.pdb
</pre>
##### Full-atom modeling results:
<pre>
Hetero-oligomer target:
CM_COMBINEi_AFonly/
CM_COMBINEi_DMonly/
CM_COMBINEi_VESPER/
CM_COMBINEi_all_rechain/
</pre>

##### Ranked Full-atom Models using DAQ and DOT score:
<pre>
ranked/
rank1.pdb
rank2.pdb
rank3.pdb
rank4.pdb
</pre>
