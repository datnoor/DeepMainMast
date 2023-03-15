import os
import sys
import glob

job_name = sys.argv[1]
hetero = sys.argv[2]

def get_daq(modelname):
    # Compute DAQ score
    daq_exec = "bin/DAQscore_Unet"
    op_dir = f"results/{job_name}/unet"
    ip_file = modelname + "pdb"
    op_file = modelname + "daq"

    os.system(f"{daq_exec} -i {op_dir} -Q {ip_file} > {op_file}")
    
    # Get DAQ score
    ave_aa=0.0
    ave_ca=0.0
    tot_aa=0.0
    tot_ca=0.0
    with open(modelname + 'daq') as f:
        for l in f:
            if l.startswith('AVG   Calpha'):
                ave_ca=float(l.split()[4])
                ave_aa=float(l.split()[6])
            if l.startswith('TOTAL Calpha'):
                tot_ca=float(l.split()[4])
                tot_aa=float(l.split()[6])

    return ave_ca,tot_ca,ave_aa,tot_aa

def get_dot(modelname):
    # Compute Dot score
    dot_exec = "bin/VESPER_Power"
    reso = 5.0
    ip_file = modelname + "pdb"
    new_map = modelname + "mrc"
    op_file = modelname + "dot"
    ip_map = f"results/{job_name}/input.map"
    
    os.system(f"python bin/btpdb2mrc.py {reso} {ip_file} {new_map}")
    os.system(f"{dot_exec} -a {ip_map} -b {new_map} -t 0.001 -T 10.0 -c 4 -g 8.0 -e -s 2.0 > {op_file}")
    
    # Get DOT score    
    vol=0.0
    dot=0.0
    with open(modelname + 'dot') as f:
        for l in f:
            if l.startswith('Overlap'):
                vol=float(l.split()[2].split('/')[1])
                dot=float(l.split()[10])
    if vol==0.0:
        return 0,0
    return dot,dot/vol

models_dict = {}
comb_files = []
if hetero: 
    comb_files = glob.glob(f"results/{job_name}/COMB*.pdb")
else:
    comb_files = glob.glob(f"results/{job_name}/COMB*rechain*.pdb")
    
for model_path in comb_files:
    daq_score = get_daq(model_path[:-3])[2]  
    dot_score = get_dot(model_path[:-3])[1]  
    models_dict[model_path] = daq_score + dot_score

models = [(mkey, models_dict[mkey]) for mkey in models_dict]
models = sorted(models, key=lambda x: x[1], reverse=True)

if not os.path.exists(f"results/{job_name}/pre_rosetta_ranked"):
    os.mkdir(f"results/{job_name}/pre_rosetta_ranked")

rank = 1
for model_path, score in models:
    print(model_path, score)
    os.system(f"cp {model_path} results/{job_name}/pre_rosetta_ranked/rank{rank}.pdb")
    rank += 1
