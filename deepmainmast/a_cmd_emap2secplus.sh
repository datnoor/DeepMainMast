ID=$1

python3 emap2secplusdmm/main.py --mode=0 -F=input/$ID/input.mrc -M=emap2secplusdmm/best_model/Unet_Protein_Atom.pth.tar --contour=0.0275 --gpu=0 --batch_size=16 --type=0

python3 emap2secplusdmm/main.py --mode=0 -F=input/$ID/input.mrc -M=emap2secplusdmm/best_model/Unet_Protein_AA.pth.tar --contour=0.0275 --gpu=0 --batch_size=16 --type=1

