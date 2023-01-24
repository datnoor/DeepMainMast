ID=$1

AF=results/$ID/af2_model.pdb

mkdir -p results/$ID/VESPER_MODELs

for reso in 5.0 6.0 7.0
do
	map=results/$ID/af2_model_R${reso}.mrc
	python3 bin/btpdb2mrc.py $reso $AF $map
done