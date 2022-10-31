ID=$1

# check input
if [ -e input/$ID/input.map ] || [ -e input/$ID/input.mrc ]; then
	if [ ! -e input/$ID/input.map ]; then
		cp input/$ID/input.mrc input/$ID/input.map	
	fi
	if [ ! -e input/$ID/input.mrc ]; then
		cp input/$ID/input.map input/$ID/input.mrc
	fi
else
	echo "ERROR: please input input.map or input.mrc or both"
fi

if [ ! -e input/$ID/seq.fasta ]; then
	echo "ERROR: please input seq.fasta"
fi

# create output directory
if [ ! -e output/$ID ]; then
	mkdir output/$ID
	mkdir output/$ID/unet
fi