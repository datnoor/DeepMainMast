ID=$1

PG=bin/MainmastC_Unet_node
DATA=output/$ID/unet
SEQ=input/$ID/seq.fasta
OUTF=output/$ID/

if [ -e $DATA/atom_CA.mrc ] && [ -e $SEQ ];then
	for p in 0.3 0.4 0.5
	do
		out=$OUTF/NODE_p$p.pdb
		if [ ! -e $out ];then
			hostname > $out
			$PG -i $DATA -s $SEQ -t $p -G > $out
		fi
	done
fi
