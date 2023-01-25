ID=$1

PG=bin/RosettaCM.py

DATA=/results/$ID/input.map
seq=/results/$ID/seq.fasta

dname=/results/$ID/CM_DMonly
xpath=bin/
model=/results/$ID/COMBINEi_DMonly.pdb
python3 $PG $seq $model $DATA --OutPath=$dname --XMLPath=$xpath

if [ -e /results/$ID/VESPER_MODELs ]; then
	for tag in AFonly VESPER all
	do
		dname=/results/$ID/CM_$tag
		model=/results/$ID/COMBINEi_${tag}.pdb
		python3 $PG $seq $model $DATA --OutPath=$dname --XMLPath=$xpath
	done
fi