ID=$1
TIME=$2

PG=bin/MainmastC_UnetAF2
ASB=bin/Assemble_Iter.py
Ncpu=2

AF=/results/$ID/af2_model.pdb
OUTF=/results/$ID

for p in 0.3 0.4 0.5
do
	for Nchain in 1 5 10
	do
		in=PATH_p${p}Nch${Nchain}.csv
		SEQ=/results/$ID/seq.fasta
		UnetDir=/results/$ID/unet

		if [ ! -e $OUTF/$in ]||[ ! -e $SEQ ]||[ ! -e $UnetDir ];then
			echo "Missing....$1 $in"
			continue
		fi


		for Nali in 5 10
		do
			INP=INP_p${p}Nch${Nchain}Nali${Nali}.txt
			OUT=OUT_p${p}Nch${Nchain}Nali${Nali}.pdb
			if [ ! -e $OUTF/$OUT ];then
			hostname > $OUTF/$OUT
			$PG -i $UnetDir -s $SEQ -G -N $OUTF/$in -l 9 -r 1.5 -c $Ncpu -T $Nali > $OUTF/$INP
			python $ASB --Nstock 10000 --Niter 3 $OUTF/$INP --OutPath $OUTF/$OUT --Ncpu $Ncpu --SecAssemble $TIME
			fi
		done
		#with AF2
		if [ -e $AF ];then
			for Nali in 5 10
			do
				for Rsize in 30 50
				do
					INP=INP_p${p}Nch${Nchain}Nali${Nali}R${Rsize}.txt
					OUT=OUT_p${p}Nch${Nchain}Nali${Nali}R${Rsize}.pdb

					if [ ! -e $OUTF/$OUT ];then
					hostname > $OUTF/$OUT
					$PG -i $UnetDir -s $SEQ -G -N $OUTF/$in -l 9 -r 1.5 -c $Ncpu -T $Nali -R $Rsize -A $AF > $OUTF/$INP
					python $ASB --Nstock 10000 --Niter 3 $OUTF/$INP --OutPath $OUTF/$OUT --Ncpu $Ncpu --SecAssemble $TIME
					fi
				done
			done
		else
			echo "Missing AF Model"
		fi
	done
done
