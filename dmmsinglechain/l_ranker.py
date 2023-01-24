import os
import sys

job_name = sys.argv[1]
num_mods = int(sys.argv[2])

def get_daq(fname):
    ave_aa=0.0
    ave_ca=0.0
    tot_aa=0.0
    tot_ca=0.0
    with open(fname) as f:
        for l in f:
            if l.startswith('AVG   Calpha'):
                ave_ca=float(l.split()[4])
                ave_aa=float(l.split()[6])
            if l.startswith('TOTAL Calpha'):
                tot_ca=float(l.split()[4])
                tot_aa=float(l.split()[6])

    return ave_ca,tot_ca,ave_aa,tot_aa

def get_dot(fname):
    vol=0.0
    dot=0.0
    with open(fname) as f:
        for l in f:
            if l.startswith('Overlap'):
                vol=float(l.split()[2].split('/')[1])
                dot=float(l.split()[10])
    if vol==0.0:
        return 0,0
    return dot,dot/vol

folders = ["CM_all", "CM_VESPER", "CM_DMonly", "CM_AFonly"]
models_dict = {}

for folder in folders:
    if not os.path.exists(f"results/{job_name}/{folder}"):
        continue
    for mnum in range(1,num_mods+1):
        model_path = f"results/{job_name}/{folder}/S_singletgt_000{mnum}."
        daq_score = get_daq(model_path + "daq")[2]  
        dot_score = get_dot(model_path + "dot")[1]  
        models_dict[model_path + "pdb"] = daq_score + dot_score

models = [(mkey, models_dict[mkey]) for mkey in models_dict]
models = sorted(models, key=lambda x: x[1], reverse=True)

if not os.path.exists(f"results/{job_name}/ranked"):
    os.mkdir(f"results/{job_name}/ranked")

rank = 1
for model_path, score in models:
    print(model_path, score)
    os.system(f"cp {model_path} results/{job_name}/ranked/rank{rank}.pdb")
    rank += 1