ID=$1
MODS=$2

PG=bin/RosettaCM.py

DATA=results/$ID/input.map
seq=results/$ID/seq.fasta

dname=results/$ID/CM_DMonly/

chmod -R 777 results/partial_thread.static.linuxgccrelease
chmod -R 777 results/rosetta_scripts.static.linuxgccrelease

echo "rosetta start"

if [ ! -e ${dname}.st ];then
	echo "Starting"
	echo $dname
	# A_setup
	cd $dname
	/content/DeepMainMast/dmmsinglechain/results/$ID/partial_thread.static.linuxgccrelease \
		-database bin/ros_database/ \
		-in::file::fasta $dname/seq.fasta \
		-in::file::alignment $dname/alignment.txt \
		-in::file::template_pdb $dname/1tmpA.pdb \
		-out:path:all $dname

	if [ -e $dname/1tmpA_thread.pdb ];then
		/content/DeepMainMast/dmmsinglechain/results/$ID/rosetta_scripts.static.linuxgccrelease \
			-database bin/ros_database/ \
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
			-default_max_cycles 200 \
			-out:path:all $dname
	fi
fi

echo "rosetta next"

if [ -e results/$ID/VESPER_MODELs ]; then
	for tag in AFonly VESPER all
	do
		dname=results/$ID/CM_$tag/
		if [ ! -e ${dname}.st ];then
			echo "Starting"
			echo $dname

			# A_setup
			cd $dname
			/content/DeepMainMast/dmmsinglechain/results/$ID/partial_thread.static.linuxgccrelease \
				-database bin/ros_database/ \
				-in::file::fasta $dname/seq.fasta \
				-in::file::alignment $dname/alignment.txt \
				-in::file::template_pdb $dname/1tmpA.pdb \
				-out:path:all $dname

			if [ -e $dname/1tmpA_thread.pdb ];then
				/content/DeepMainMast/dmmsinglechain/results/$ID/rosetta_scripts.static.linuxgccrelease \
					-database bin/ros_database/ \
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
					-default_max_cycles 200 \
					-out:path:all $dname
			fi
		fi
	done
fi