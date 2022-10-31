ID=$1

PG=bin/RosettaCM.py

DATA=input/$ID/input.map
seq=input/$ID/seq.fasta

dname=output/$ID/CM_all
xpath=bin/
model=output/$ID/COMBINEi_all.pdb
python3 $PG $seq $model $DATA --OutPath=$dname --XMLPath=$xpath

if [ -e output/$ID/VESPER_MODELs ]; then
	for tag in AFonly DMonly VESPER
	do
		dname=output/$ID/CM_$tag
		model=output/$ID/COMBINEi_${tag}.pdb
		python3 $PG $seq $model $DATA --OutPath=$dname --XMLPath=$xpath
	done
fi