import os
import subprocess
import numpy as np
from Bio.PDB import *
import random
import string
import argparse
import copy


def get_args(): 
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("FASTA", type=str, help="Target Sequence")
    parser.add_argument("MODEL", type=str, help="Model(PDB format)")
    parser.add_argument("MAP", type=str, help="MAP")
    parser.add_argument('--OutPath', type=str, dest='OutPath', default='./OutPutDir', help='Out put path. Please use Dir name such as ./OUTPUTs')
    parser.add_argument('--XMLPath', type=str, dest='XMLPath', default='/net/kihara/home/gterashi/DeepMainmast/BenchMark_CRI_TASSER/bin/', help='Copy *.xml and *.sh files')
    args = parser.parse_args()
    return args

def main():
    d3to1 = {'CYS': 'C', 'ASP': 'D', 'SER': 'S', 'GLN': 'Q', 'LYS': 'K',
             'ILE': 'I', 'PRO': 'P', 'THR': 'T', 'PHE': 'F', 'ASN': 'N', 
             'GLY': 'G', 'HIS': 'H', 'LEU': 'L', 'ARG': 'R', 'TRP': 'W', 
             'ALA': 'A', 'VAL':'V', 'GLU': 'E', 'TYR': 'Y', 'MET': 'M'}
    #cmd=['ln','-s',map_path,input_map]
    #res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')

    args = get_args()
    MODEL=args.MODEL
    OutPath=args.OutPath
    FASTA=args.FASTA
    MAP=args.MAP
    if not os.path.isfile(FASTA):
        print('Can not find',FASTA)
        return 0
    if not os.path.isfile(MAP):
        print('Can not find',MAP)
        return 0
    if not os.path.isfile(MAP):
        print('Can not find',MAP)
        return 0
    
    if os.mkdir(OutPath):
        print('Directory is existing..',OutPath)
        return 0
    
    
    model = PDBParser().get_structure("model",MODEL)[0]
    
    id_name,seq = ReadFasta(FASTA)
    
    print(id_name,seq)
    #get first chain
    for chain in model:
        break
    temp_seq=''
    for pos in range(len(seq)):
        print(pos,seq[pos])
        if pos+1 in chain:
            print(d3to1[chain[pos+1].resname])
            temp_seq=temp_seq+d3to1[chain[pos+1].resname]
        else:
            temp_seq=temp_seq+'-'
            
    #Make Alignment file
    alignment_file=OutPath+'/alignment.txt'
    lines='## 1XXX_ 1tmpA_thread\n'
    lines=lines+'# hhsearch\n'
    lines=lines+'scores_from_program: 0 1.00\n'
    lines=lines+'0 '+seq+'\n'
    lines=lines+'0 '+temp_seq+'\n--\n'
    print(seq)
    print(temp_seq)
    with open(alignment_file,'w') as out:
        out.write(lines)
        
        
    out_map = OutPath + '/inputmap.map'
    cmd=['cp',MAP,out_map]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    out_fasta = OutPath + '/seq.fasta'
    cmd=['cp',FASTA,out_fasta]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    out_model = OutPath + '/input.pdb'
    cmd=['cp',MODEL,out_model]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    cmd=['bin/pulchra','-s',out_model]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    out_model = OutPath + '/input.rebuilt.pdb'
    out_model2 = OutPath + '/1tmpA.pdb'
    cmd=['cp',out_model,out_model2]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    XMLPath=args.XMLPath
    out_model = XMLPath + '/C_rosettaCM.sh'
    out_model2 = OutPath + '/.'
    cmd=['cp',out_model,out_model2]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    out_model = XMLPath + '/C_rosettaCM.xml'
    out_model2 = OutPath + '/.'
    cmd=['cp',out_model,out_model2]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
    
    out_model = XMLPath + '/A_setup.sh'
    out_model2 = OutPath + '/.'
    cmd=['cp',out_model,out_model2]
    res=subprocess.run(cmd,stdout=subprocess.PIPE,encoding='utf-8')
            

    
def ReadFasta(file):
    seq=''
    name=''
    with open(file) as f:
        for l in f:
            l=l.strip()
            if l.startswith('>'):
                name=l
            else:
                seq = seq + l
    return name,seq
           


    
if __name__ == '__main__':
    main()