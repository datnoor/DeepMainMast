ID=$1

PG=bin/RosettaCM.py

DATA=input/$ID/input.map
seq=input/$ID/seq.fasta

dname=output/$ID/CM_all

if [ ! -e ${dname}.st ];then
	# A_setup
	bin/partial_thread.static.linuxgccrelease \
		-database bin/ros_database/ \
		-in::file::fasta $dname/seq.fasta \
		-in::file::alignment $dname/alignment.txt \
		-in::file::template_pdb $dname/1tmpA.pdb
	
	if [ -e 1tmpA_thread.pdb ];then
		bin/rosetta_scripts.static.linuxgccrelease \
		-database bin/ros_database/ \
		-in:file:fasta $dname/seq.fasta \
		-parser:protocol $dname/C_rosettaCM.xml \
		-nstruct 5 \
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

mv 1tmpA_thread.pdb $dname
mv S_singletgt_0001.pdb $dname
mv S_singletgt_0002.pdb $dname
mv S_singletgt_0003.pdb $dname
mv S_singletgt_0004.pdb $dname
mv S_singletgt_0005.pdb $dname
mv score_singletgt.sc $dname

if [ -e output/$ID/VESPER_MODELs ]; then
	for tag in AFonly DMonly VESPER
	do
		dname=output/$ID/CM_$tag
		if [ ! -e ${dname}.st ];then
			# A_setup
			bin/partial_thread.static.linuxgccrelease \
				-database bin/ros_database/ \
				-in::file::fasta $dname/seq.fasta \
				-in::file::alignment $dname/alignment.txt \
				-in::file::template_pdb $dname/1tmpA.pdb

				
			if [ -e 1tmpA_thread.pdb ];then
				bin/rosetta_scripts.static.linuxgccrelease \
				-database bin/ros_database/ \
				-in:file:fasta $dname/seq.fasta \
				-parser:protocol $dname/C_rosettaCM.xml \
				-nstruct 5 \
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

		mv 1tmpA_thread.pdb $dname
		mv S_singletgt_0001.pdb $dname
		mv S_singletgt_0002.pdb $dname
		mv S_singletgt_0003.pdb $dname
		mv S_singletgt_0004.pdb $dname
		mv S_singletgt_0005.pdb $dname
		mv score_singletgt.sc $dname

	done
fi