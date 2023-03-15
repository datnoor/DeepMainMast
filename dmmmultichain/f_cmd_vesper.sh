ID=$1

PG=bin/VESPER_Power
PG2=bin/RotateVesper.py

data=results/$ID/input.map
seq=results/$ID/seq.fasta

AF=results/$ID/af2_model.pdb

opdir=results/$ID/VESPER_MODELs

if [ ! -e results/$ID/vesper_R5.0.out ];then
	hostname > results/$ID/vesper_R5.0.out
	for reso in 5.0 6.0 7.0
	do
		for chid in A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
		do
			input_model=results/$ID/VESPER_MODELs/af2_${chid}.pdb
			map=results/$ID/VESPER_MODELs/af2_${chid}_R${reso}.mrc
			outfile=results/$ID/VESPER_MODELs/af2_${chid}_R${reso}.out
			if [ ! -e $outfile ] && [ -e $map ] && [ -e $data ];then
				hostname > $outfile
				$PG -a $data -b $map -t 0.001 -T 10.0 -c 4 -g 8.0 -s 1.0 > $outfile
				python3 $PG2 $outfile $input_model --OutPath results/$ID/VESPER_MODELs/R${reso}_${chid}_
			fi			
		done
	done
fi