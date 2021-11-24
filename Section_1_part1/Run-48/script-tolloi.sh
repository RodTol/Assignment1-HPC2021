#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=1:ppn=24

cd /u/dssc/tolloi/Assignment1-HPC2021/Section_1_part2
module load   openmpi/4.0.3/gnu/9.3.0
module load   likwid/5.1.0

mpirun --mca btl ^openib -np 24 matrix_arrays.x 26 2 1 2>/dev/null

likwid-topology -g
cat $PBS_NODEFILE