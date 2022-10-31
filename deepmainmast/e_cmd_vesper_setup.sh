ID=$1

AF=input/$ID/afmodel/af2_model.pdb

mkdir output/$ID/VESPER_MODELs

for reso in 5.0 6.0 7.0
do
	map=output/$ID/af2_model_R${reso}.mrc
	bin/btpdb2mrc.py -R $reso $AF $map
	#$PG -a ./inputmap.map -b $map -t 0.001 -T 10.0 -c 4 -g 8.0 > vesper_R${reso}.out
	#python3 $PG2 vesper_R${reso}.out ./af2_model.pdb --OutPath ./VESPER_MODELs/R${reso}_
done
