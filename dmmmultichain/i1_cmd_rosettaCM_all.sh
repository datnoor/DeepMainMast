ID=$1
MODS=$2

PG=/code/bin/RosettaCM.py

DATA=/results/$ID/input.map
seq=/results/$ID/seq.fasta

chmod -R 777 /results/partial_thread.static.linuxgccrelease
chmod -R 777 /results/rosetta_scripts.static.linuxgccrelease

for dname in /results/$ID/CM_*_rechain_*[01]
do
	if [ ! -e ${dname}.st ] && [ -e $dname ];then
		hostname > ${dname}.st 
		# A_setup
		cd $dname
		/results/partial_thread.static.linuxgccrelease \
			-database /code/bin/ros_database/ \
			-in::file::fasta $dname/seq.fasta \
			-in::file::alignment $dname/alignment.txt \
			-in::file::template_pdb $dname/1tmpA.pdb

		if [ -e 1tmpA_thread.pdb ];then
			/results/rosetta_scripts.static.linuxgccrelease \
				-database /code/bin/ros_database/ \
				-in:file:fasta $dname/seq.fasta \
				-parser:protocol $dname/C_rosettaCM.xml \
				-nstruct $MODS \
				-relax:jump_move true \
				-relax:dualspace \
				-out::suffix _singletgt \
				-edensity::mapfile $dname/inputmap.map \
				-edensity::mapreso 5.0 \
				-edensity::cryoem_scatterers \
				-beta \
				-default_max_cycles 200
		fi
	fi
done
