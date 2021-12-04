#!/bin/bash
#PBS -q dssc
#PBS -l walltime=1:00:00
#PBS -l nodes=2:ppn=2

cd /u/dssc/tolloi/Assignment1-HPC2021/Section3

module load   openmpi/4.0.3/gnu/9.3.0
module load   likwid/5.1.0

