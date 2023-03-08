ID=$1

PG=bin/RosettaCM.py

DATA=results/$ID/input.map
seq=results/$ID/seq.fasta
xpath=bin/

for model in results/$ID/COMB*_rechain*.pdb
do
	tag=`basename $model .pdb`
	dname=results/$ID/CM_$tag
	chk=`wc -l $model|perl -alne 'print $F[0]'`
	if [ -e $model ] && [ $chk -gt 10 ] && [ ! -e $dname ];then
		##Split Chains and PULCHRA
		##Make Alignment and threading files for Rosetta		
		python3 $PG $seq $model $DATA --OutPath=$dname --XMLPath=$xpath
	fi
done
