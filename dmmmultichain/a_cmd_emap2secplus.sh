ID=$1
CONTOUR=$2

python3 emap2secplus/main.py --mode=0 -F=/results/$ID/input.mrc -M=emap2secplus/best_model/Unet_Protein_Atom.pth.tar --contour=$CONTOUR --gpu=0 --batch_size=16 --type=0

python3 emap2secplus/main.py --mode=0 -F=/results/$ID/input.mrc -M=emap2secplus/best_model/Unet_Protein_AA.pth.tar --contour=$CONTOUR --gpu=0 --batch_size=16 --type=1

