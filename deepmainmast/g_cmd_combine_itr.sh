ID=$1

PG_ASB=bin/MainmastC_UnetAssembleMtx
PG=bin/Assemble_Iter.py #combine all models
DATA=output/$ID/unet
OUTF=output/$ID/
SEQ=input/$ID/seq.fasta

#Make input files
modelfile1=$OUTF/MODELs_all.pdb
modelfile2=$OUTF/MODELs_DMonly.pdb
modelfile3=$OUTF/MODELs_AFonly.pdb
modelfile4=$OUTF/MODELs_VESPER.pdb

comb_model1=$OUTF/COMBINEi_all.pdb
comb_model2=$OUTF/COMBINEi_DMonly.pdb
comb_model3=$OUTF/COMBINEi_AFonly.pdb
comb_model4=$OUTF/COMBINEi_VESPER.pdb

if [ ! -e $OUTF/comb.st ];then
	hostname > $OUTF/comb.st
	#Sigmoid Scoring
	if [ ! -e $modelfile1 ];then
		for p in $OUTF/OUT_p*.pdb
		do
			cat $p
			echo "TER"
		done > $modelfile1
	fi
	if [ ! -e $modelfile2 ];then
		for p in $OUTF/OUT_p*ali??.pdb $OUTF/OUT_p*ali?.pdb
		do
			cat $p
			echo "TER"
		done > $modelfile2
	fi
	if [ ! -e $modelfile3 ];then
		if [ -e $OUTF/VESPER_MODELs ];then
			for p in $OUTF/OUT_p*R*.pdb
			do
				cat $p
				echo "TER"
			done > $modelfile3
		else
			echo "Running without AF model output"
		fi	
	fi
	if [ ! -e $modelfile4 ];then
		if [ -e $OUTF/VESPER_MODELs ];then
			for p in $OUTF/VESPER_MODELs/*.pdb
			do
				cat $p
				echo "TER"
			done > $modelfile4
		else
			echo "Running without AF model output"
		fi
	fi
	#l=10
	mtxfile1=$OUTF/MTX_all.txt
	mtxfile2=$OUTF/MTX_DMonly.txt
	mtxfile3=$OUTF/MTX_AFonly.txt
	mtxfile4=$OUTF/MTX_VESPER.txt

	if [ ! -e $mtxfile1 ];then
		$PG_ASB -i $DATA -s $SEQ -l 10 -A $modelfile1 -G > $mtxfile1
	fi
	if [ ! -e $mtxfile2 ];then
		$PG_ASB -i $DATA -s $SEQ -l 10 -A $modelfile2 -G > $mtxfile2
	fi
	if [ -e $OUTF/VESPER_MODELs ];then
		if [ ! -e $mtxfile3 ];then
			$PG_ASB -i $DATA -s $SEQ -l 10 -A $modelfile3 -G > $mtxfile3
		fi
		if [ ! -e $mtxfile4 ];then
			$PG_ASB -i $DATA -s $SEQ -l 10 -A $modelfile4 -G > $mtxfile4
		fi
	else
		echo "Running without AF model output"
	fi

	if [ ! -e $comb_model1 ];then
		python3 $PG $mtxfile1 --Nstock 10000 --OutPath $comb_model1 --SecAssemble 300 --Ncpu 4
		echo "DEBUG 1"
	fi
	if [ ! -e $comb_model2 ];then
		python3 $PG $mtxfile2 --Nstock 10000 --OutPath $comb_model2 --SecAssemble 300 --Ncpu 4
		echo "DEBUG 2"
	fi
	if [ -e $OUTF/VESPER_MODELs ];then
		if [ ! -e $comb_model3 ];then
			python3 $PG $mtxfile3 --Nstock 10000 --OutPath $comb_model3 --SecAssemble 300 --Ncpu 4
		fi
		if [ ! -e $comb_model4 ];then
			python3 $PG $mtxfile4 --Nstock 10000 --OutPath $comb_model4 --SecAssemble 300 --Ncpu 4
		fi
	else
		echo "Running without AF model output"
	fi
fi