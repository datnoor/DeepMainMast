ID=$1

PG=bin/MainmastC_Unet_node
DATA=results/$ID/unet
SEQ=results/$ID/seq.fasta
OUTF=results/$ID/

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
