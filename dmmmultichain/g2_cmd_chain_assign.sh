ID=$1
TIME=$2

PG_ASB=bin/MainmastC_UnetChainAssign
PG=bin/Assemble_ReChainID.py #combine all models

DATA=results/$ID/unet
OUTF=results/$ID/
SEQ=results/$ID/seq.fasta

Ncpu=4
Filter=-10.0

dcut=3.0
mtxfile1=$OUTF/CH_all_d$dcut.txt
mtxfile2=$OUTF/CH_DMonly_d$dcut.txt
mtxfile3=$OUTF/CH_AFonly_d$dcut.txt
mtxfile4=$OUTF/CH_VESPER_d$dcut.txt

w=1.0
comb_model1=$OUTF/COMBINEi_all_rechain_d${dcut}_w$w.pdb
comb_model2=$OUTF/COMBINEi_DMonly_rechain_d${dcut}_w$w.pdb
comb_model3=$OUTF/COMBINEi_AFonly_rechain_d${dcut}_w$w.pdb
comb_model4=$OUTF/COMBINEi_VESPER_rechain_d${dcut}_w$w.pdb

if [ ! -e $comb_model1 ];then
	hostname > $comb_model1
	python $PG $mtxfile1 --HomWeight $w --Nstock 10000 --OutPath $comb_model1 --SecAssemble $TIME --Ncpu $Ncpu
fi
if [ ! -e $comb_model2 ];then
	hostname > $comb_model2
	python $PG $mtxfile2 --HomWeight $w --Nstock 10000 --OutPath $comb_model2 --SecAssemble $TIME --Ncpu $Ncpu
fi
if [ ! -e $comb_model3 ];then
	hostname > $comb_model3
	python $PG $mtxfile3 --HomWeight $w --Nstock 10000 --OutPath $comb_model3 --SecAssemble $TIME --Ncpu $Ncpu
fi
if [ ! -e $comb_model4 ];then
	hostname > $comb_model4
	python $PG $mtxfile4 --HomWeight $w --Nstock 10000 --OutPath $comb_model4 --SecAssemble $TIME --Ncpu $Ncpu
fi

