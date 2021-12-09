#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=2:ppn=12

cd /u/dssc/tolloi/Assignment1-HPC2021/Section_1_part2

module load   openmpi/4.0.3/gnu/9.3.0
module load   likwid/5.1.0

echo "Matrice 2400 100 100 "
mpirun --mca btl ^openib -np 24 matrix3d.x 2400 100 100

echo "Matrice 1200 200 100 "
mpirun --mca btl ^openib -np 24 matrix3d.x 1200 200 100

echo "Matrice 800 300 100 "
mpirun --mca btl ^openib -np 24 matrix3d.x 800 300 100

likwid-topology -g
cat $PBS_NODEFILE