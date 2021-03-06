#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=1:ppn=24

cd /u/dssc/tolloi/Assignment1-HPC2021/Section_1_part1/Run-prova
module load   openmpi/4.0.3/gnu/9.3.0
module load   likwid/5.1.0

for procs in {2..24} ; do
echo "executing on ", ${procs}, "  processors" 
mpirun -np ${procs} ring.x >out.${procs} 2>/dev/null
done 
likwid-topology -g
cat $PBS_NODEFILE