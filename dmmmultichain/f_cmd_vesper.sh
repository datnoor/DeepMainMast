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
		map=results/$ID/af2_model_R${reso}.mrc
		if [ -e $map ];then
			$PG -a $data -b $map -t 0.001 -T 10.0 -c 4 -g 8.0 -s 1.0 > results/$ID/vesper_R${reso}.out
			python3 $PG2 results/$ID/vesper_R${reso}.out $AF --OutPath $opdir/R${reso}_
		fi

	done
fi