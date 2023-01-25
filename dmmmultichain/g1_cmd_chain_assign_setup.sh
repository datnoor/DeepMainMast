ID=$1
TIME=$2

PG_ASB=/code/bin/MainmastC_UnetChainAssign
PG=/code/bin/Assemble_ReChainID.py #combine all models

DATA=/results/$ID/unet
OUTF=/results/$ID/
SEQ=/results/$ID/seq.fasta

Ncpu=4
Filter=-10.0

inp_model1=$OUTF/COMBINEi_all.pdb
inp_model2=$OUTF/COMBINEi_DMonly.pdb
inp_model3=$OUTF/COMBINEi_AFonly.pdb
inp_model4=$OUTF/COMBINEi_VESPER.pdb

dcut=3.0

DiffCut=$dcut
mtxfile1=$OUTF/CH_all_d$dcut.txt
mtxfile2=$OUTF/CH_DMonly_d$dcut.txt
mtxfile3=$OUTF/CH_AFonly_d$dcut.txt
mtxfile4=$OUTF/CH_VESPER_d$dcut.txt

if [ ! -e $mtxfile1 ] && [ -e $inp_model1 ];then
	$PG_ASB -i $DATA -s $SEQ -A $inp_model1 -H -f $Filter -r $DiffCut > $mtxfile1
fi
if [ ! -e $mtxfile2 ] && [ -e $inp_model2 ];then
	$PG_ASB -i $DATA -s $SEQ -A $inp_model2 -H -f $Filter -r $DiffCut > $mtxfile2
fi
if [ ! -e $mtxfile3 ] && [ -e $inp_model3 ];then
	$PG_ASB -i $DATA -s $SEQ -A $inp_model3 -H -f $Filter -r $DiffCut > $mtxfile3
fi
if [ ! -e $mtxfile4 ] && [ -e $inp_model4 ];then
	$PG_ASB -i $DATA -s $SEQ -A $inp_model4 -H -f $Filter -r $DiffCut > $mtxfile4
fi