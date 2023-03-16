ID=$1

PG=bin/VESPER_Power
PG2=bin/RotateVesper.py
PG3=bin/SplitPDBchain.py

AF=results/$ID/af2_model.pdb

mkdir -p results/$ID/VESPER_MODELs

#Split PDB to chains
python3 $PG3 $AF --OutPath=results/$ID/VESPER_MODELs/af2

for reso in 5.0 6.0 7.0
do
	for chid in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
	do
		input_model=results/$ID/VESPER_MODELs/af2_${chid}.pdb
		if [ -e $input_model ];then
			map=results/$ID/VESPER_MODELs/af2_${chid}_R${reso}.mrc
			if [ ! -e $map ];then
				python3 bin/btpdb2mrc.py $reso $input_model $map
			fi
		fi
	done
done