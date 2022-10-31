ID=$1

PG_DAQ=bin/DAQscore_Unet
DATA=output/$ID/unet

for model in output/$ID/CM_*/S_*.pdb
do
	name=`echo $model|sed -e s/.pdb$//`
	echo $name
	new_file=${name}.daq
	if [ -e $model ] && [ ! -e $new_file ];then
		echo $new_file
		$PG_DAQ -i $DATA -Q $model > $new_file
	fi
done
