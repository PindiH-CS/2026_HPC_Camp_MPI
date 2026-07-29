#!/usr/bin/bash
# Slurm script. Change the account and partition to match your cluster.
# Not on Slurm? Generate your own testcase instead:
#   python gen_testcase.py gen 1000 t01
#   mpirun -n 4 ./parallel_merge_sort 1000 t01.in t01.result
#   python gen_testcase.py check t01.result t01.out
#SBATCH -A GOV115003
#SBATCH -p development
#SBATCH -n 16
#SBATCH --exclusive
#SBATCH -J merge_sort
#SBATCH -e %j.e
#SBATCH -o %j.out

module purge
module load gcc/10.4.0 openmpi/5.0.2

# Point this at your testcase directory, e.g.
#   TESTCASE_DIR=/path/to/testcases CASE=01 sbatch job.sh
if [ -z "$TESTCASE_DIR" ]; then
    echo "set TESTCASE_DIR to the directory holding the .in/.txt/.out files" >&2
    exit 1
fi
CASE=${CASE:-01}              # which testcase to run

# The .txt holds n (element count) and nodes (how many processes to run) --
# read both from it. NP=<n> on the command line overrides the process count.
TXT="$TESTCASE_DIR/$CASE.txt"
cat "$TXT"
# split each line on ':'/'=' + whitespace, then match the first field by name
field() { awk '{s=$0; gsub(/[:=]/," ",s); split(s,a," "); if(tolower(a[1])==k){print a[2]; exit}}' k="$1" "$2"; }
N=$(field n "$TXT")
NP=${NP:-$(field nodes "$TXT")}
if [ -z "$N" ] || [ -z "$NP" ]; then
    echo "could not read n / nodes from $TXT" >&2
    exit 1
fi
echo "n=$N  np=$NP"          # NP must fit the allocation (#SBATCH -n above)

# mpirun -n <# of processes> ./parallel_merge_sort <n> <input> <output>
time mpirun -n "$NP" ./parallel_merge_sort "$N" "$TESTCASE_DIR/$CASE.in" "./$CASE.result"

# Verify the result against the expected output.
python gen_testcase.py check "./$CASE.result" "$TESTCASE_DIR/$CASE.out"

# To eyeball the file:
#   od -t f4 ./$CASE.result | head
