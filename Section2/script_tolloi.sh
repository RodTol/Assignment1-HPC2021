#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=ct1pt-tnode007:ppn=2 -l nodes=ct1pt-tnode009:ppn=2

cd /u/dssc/tolloi/Intel-benchmarks/src_c
module load   openmpi/4.0.3/gnu/9.3.0
module load   likwid/5.1.0

make clean
make

echo "executing run by core infiniband"
mpirun -np 2 --map-by core  --report-bindings ./IMB-MPI1 PingPong -msglog 29
echo "executing run by socket infiniband"
mpirun -np 2 --map-by socket  --report-bindings ./IMB-MPI1 PingPong -msglog 29
echo "executing run by node infiniband"
mpirun -np 2 --map-by node --report-bindings ./IMB-MPI1 PingPong -msglog 29
echo "Ethernet da qui in poi"
echo "executing run by core ethernet"
mpirun -np 2 --mca pml ob1 --report-bindings  --map-by core --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29
echo "executing run by socket ethernet"
mpirun -np 2 --mca pml ob1 --report-bindings  --map-by socket --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29
echo "executing run by node ethernet"
mpirun -np 2 --mca pml ob1 --report-bindings  --map-by node --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29

make clean

module unload  openmpi/4.0.3/gnu/9.3.0
module load intel/20.4

make
echo "------------------------INTEL------------------------"

echo "executing run by core infiniband"
mpirun -np 2 -ppn=2 -env I_MPI_DEBUG 5 -genv I_MPI_PIN_PROCESSOR_LIST 0,2  ./IMB-MPI1 PingPong -msglog 29
echo "executing run by socket infiniband"
mpirun -np 2 -ppn=2 -env I_MPI_DEBUG 5 -genv I_MPI_PIN_PROCESSOR_LIST 0,1  ./IMB-MPI1 PingPong -msglog 29
echo "executing run by node infiniband"
mpirun -np 2 -ppn=1 -env I_MPI_DEBUG 5 -host ct1pt-tnode007,ct1pt-tnode009 ./IMB-MPI1 PingPong -msglog 29
echo "Ethernet da qui in poi---------------"
#echo "executing run by core ethernet"
#mpirun -np 2 --mca pml ob1 --report-bindings  --map-by core --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29
#echo "executing run by socket ethernet"
#mpirun -np 2 --mca pml ob1 --report-bindings  --map-by socket --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29
#echo "executing run by node ethernet"
#mpirun -np 2 --mca pml ob1 --report-bindings  --map-by node --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29

make clean


likwid-topology -g
cat $PBS_NODEFILE