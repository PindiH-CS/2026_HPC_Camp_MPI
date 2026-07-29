#!/usr/bin/bash
#SBATCH -A GOV115003
#SBATCH -p development
#SBATCH -n 10
#SBATCH --exclusive
#SBATCH -J pi_calc
#SBATCH -e %j.e
#SBATCH -o %j.out

module purge
module load gcc/10.4.0 openmpi/5.0.2

# mpirun -n <# of processes> ./pi_calc <# of tests>
time mpirun -n 10 ./pi_calc 10000

# --- Observe-----------------------------------------------------------------
# Fix the process count, vary the test count -> watch the accuracy of pi
#   time mpirun -n 10 ./pi_calc 10000
#   time mpirun -n 10 ./pi_calc 1000000
#   time mpirun -n 10 ./pi_calc 100000000
#
# Fix the test count, vary the process count -> watch the elapsed time
#   (remember to change #SBATCH -n above to match)
#   time mpirun -n 1  ./pi_calc 100000000
#   time mpirun -n 2  ./pi_calc 100000000
#   time mpirun -n 4  ./pi_calc 100000000
#   time mpirun -n 8  ./pi_calc 100000000
# ----------------------------------------------------------------------------
