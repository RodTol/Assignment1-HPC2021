#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=2:ppn=24

cd /u/dssc/tolloi/Assignment1-HPC2021/Section3

module load   openmpi/4.0.3/gnu/9.3.0
module load   likwid/5.1.0

echo "Serial"
mpirun --mca btl ^openib -np 1 --map-by core ./jacoby3D.x <input.1200

echo "Same socket 4 cores"
mpirun --mca btl ^openib -np 4 --map-by core ./jacoby3D.x <input.1200

echo "Same socket 8 cores"
mpirun --mca btl ^openib -np 8 --map-by core ./jacoby3D.x <input.1200

echo "Same socket 12 cores"
mpirun --mca btl ^openib -np 12 --map-by core ./jacoby3D.x <input.1200

echo "Two sockets 4 cores"
mpirun --mca btl ^openib -np 4 --map-by socket ./jacoby3D.x <input.1200

echo "Two sockets 8 cores"
mpirun --mca btl ^openib -np 8 --map-by socket ./jacoby3D.x <input.1200

echo "Two sockets 12 cores"
mpirun --mca btl ^openib -np 12 --map-by socket ./jacoby3D.x <input.1200

echo "Two nodes 12 cores"
mpirun --mca btl ^openib -np 12 --map-by node ./jacoby3D.x <input.1200

echo "Two nodes 24 cores"
mpirun --mca btl ^openib -np 24 --map-by node ./jacoby3D.x <input.1200

echo "Two nodes 48 cores"
mpirun --mca btl ^openib -np 48 --map-by node ./jacoby3D.x <input.1200

likwid-topology -g
cat $PBS_NODEFILE