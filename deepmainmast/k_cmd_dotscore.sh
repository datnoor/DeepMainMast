ID=$1

PG=bin/VESPER_Power
PG2=bin/RotateVesper.py

reso=5.0

echo $ID
data=input/$ID/input.map

for model in output/$ID/CM_*/S_*.pdb
do
    name=`echo $model|sed -e s/.pdb$//`
    new_file=${name}.dot
    new_map=${name}.mrc
    if [ -e $model ] && [ ! -e $new_file ];then
        hostname > $new_file
        bin/btpdb2mrc.py -R $reso $model $new_map 
        $PG -a $data -b $new_map -t 0.001 -T 10.0 -c 4 -g 8.0 -e -s 2.0 > $new_file
    fi
done
