ID=$1

# create output directory
if [ ! -e results/$ID ]; then
	mkdir results/$ID
	mkdir results/$ID/unet
fi

# check input
if [ -e data/$ID/input.map ] || [ -e data/$ID/input.mrc ]; then
	if [ -e data/$ID/input.map ]; then
		cp data/$ID/input.map results/$ID/input.map	
		cp data/$ID/input.map results/$ID/input.mrc
	else
		cp data/$ID/input.mrc results/$ID/input.mrc
		cp data/$ID/input.mrc results/$ID/input.map
	fi
else
	echo "ERROR: please input input.map or input.mrc or both"
fi

if [ ! -e data/$ID/seq.fasta ]; then
	echo "ERROR: please input seq.fasta"
else
	cp data/$ID/seq.fasta results/$ID/seq.fasta
fi

if [ -e data/$ID/af2_model.pdb ]; then
	cp data/$ID/af2_model.pdb results/$ID/af2_model.pdb
fi
