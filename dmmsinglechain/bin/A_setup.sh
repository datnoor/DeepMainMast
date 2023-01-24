#!/bin/bash

bin/partial_thread.static.linuxgccrelease \
 -database bin/ros_database/ \
 -in::file::fasta seq.fasta \
 -in::file::alignment alignment.txt \
 -in::file::template_pdb 1tmpA.pdb
