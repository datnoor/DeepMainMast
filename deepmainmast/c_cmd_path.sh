ID=$1

PG=bin/MainmastC_Unet_node
ASB=bin/UnetPath.py

OUTF=output/$ID/

for p in 0.3 0.4 0.5
do
	for Nchain in 1 5 10
	do
		in=$OUTF/NODE_p${p}.pdb
		out=$OUTF/PATH_p${p}Nch${Nchain}.csv
		log=$OUTF/PATH_p${p}Nch${Nchain}.log

		if [ ! -e $log ];then
		hostname > $log
		# python $ASB $in --Nchain $Nchain --OutCSV $out --SecTrace 180 > $log
		python $ASB $in --Nchain $Nchain --OutCSV $out --SecTrace 60 --SecAssemble 60 > $log
		fi
		
	done
done
