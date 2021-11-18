#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=ct1pt-tnode008:ppn=24

cd /u/dssc/tolloi/Assignment1-HPC2021
module load   openmpi/4.0.3/gnu/9.3.0

for procs in {2..24} ; do
echo "executing on ", ${procs}, "  processors" 
mpirun -np ${procs} ring.x >out.${procs}
done 