# MPI Lab — NTHU HPCAI Summer Camp 2026
```
lab1-hello/
  mpi_hello_world.cc       uncomment the MPI calls, then compile and run
  mpi_send_recv.cc         TODOs
lab2-pi/
  pi_calc.cc               single-process; distribute it with MPI
  job.sh                   Slurm script
hw-mergesort/
  parallel_merge_sort.cc   TODOs
  gen_testcase.py          generate / verify testcases locally
  job.sh                   Slurm script + result check
```

## Building

Any MPI implementation with a C++17 compiler works. There are deliberately no
Makefiles — driving `mpicxx` yourself is part of the exercise. It is the usual
compiler wrapper, so it takes the flags you already know:

```bash
cd lab1-hello
mpicxx -o mpi_hello_world mpi_hello_world.cc
```

`mpicxx --showme` (Open MPI) prints the underlying compiler command if you want
to see what it expands to.

On the camp cluster, load the modules first:

```bash
module purge
module load gcc/10.4.0 openmpi/5.0.2
mpicxx --version
```

---

## Lab 1 — compiling and running an MPI program

```bash
cd lab1-hello
mpicxx -o mpi_hello_world mpi_hello_world.cc
mpicxx -o mpi_send_recv   mpi_send_recv.cc
mpirun -np 4 ./mpi_hello_world
mpirun -np 2 ./mpi_send_recv
```

`mpi_hello_world.cc` has its MPI calls commented out: uncomment them, then get
your environment loaded and the program compiled and running. The four calls
every MPI program needs:

| Call | What it does |
| --- | --- |
| `MPI_Init` | starts MPI; almost nothing MPI may be used before it |
| `MPI_Comm_rank` | which process am I, `0 .. size-1` |
| `MPI_Comm_size` | how many processes are there |
| `MPI_Finalize` | tears MPI down; nothing MPI may follow it |

`MPI_COMM_WORLD` is the communicator containing every process `mpirun`
started; a rank is only meaningful relative to a communicator.

**"Do the processes print in order?"** No. `mpirun` starts independent
processes and nothing orders them, so the lines race each other and the order
changes between runs. To get ordered output you have to build the order
yourself — gather to rank 0, or pass a token down the ranks. `MPI_Barrier`
does *not* do it: it only makes everyone arrive at the same point, it says
nothing about when their output reaches the terminal.

`mpi_send_recv.cc` is the point-to-point exercise: make rank 0
send `"Hello, Process 1"` to rank 1. Note `strlen(message) + 1` — the
terminating `'\0'` has to travel too — and that `MPI_Recv`'s count is your
buffer's *capacity*, not the exact size of the incoming message.

---

## Lab 2 — Monte Carlo Pi

Scatter random points in a square; the fraction landing inside the inscribed
circle is `pi/4`, so `pi ≈ 4 × hits / tests`.

`pi_calc.cc` starts as a **working single-process** program. Your job is to distribute it with MPI:

```bash
cd lab2-pi
mpicxx -o pi_calc pi_calc.cc

sbatch job.sh
```

Split the loop across ranks, then gather the per-rank counts back together
(`MPI_Reduce`). Two easy things to get wrong:

- **Each rank needs its own random seed.** With the same seed every process
  generates an identical sequence and you have computed the same answer P
  times instead of P times as many samples.
- **`tests` must be `long long`** — `int` overflows just
  past 2×10⁹.

### What you should observe

| Change | Effect |
| --- | --- |
| more tests | pi gets more accurate; error ~ `O(1/√N)`, so each extra digit costs 100× the samples |
| more processes | runtime drops close to linearly, then saturates once MPI start-up and communication stop being negligible |

---

## Homework — parallel merge sort

Binary tree merge

```
P0 P1 P2 P3 P4 P5 P6 P7    <- local sort
 \ /   \ /   \ /   \ /
 P0    P2    P4    P6      <- step = 1
  \   /       \   /
   P0          P4          <- step = 2
     \        /
         P0                <- step = 4, the answer is on rank 0
```

```bash
cd hw-mergesort
mpicxx -o parallel_merge_sort parallel_merge_sort.cc
mpirun -n 4 ./parallel_merge_sort <n> <input> <output>
```

Like lab 2, the template **already works sequentially**: rank 0 reads the
whole array with MPI-IO, `std::sort`s it and writes it back out, so
`mpirun -n 1` passes the check before you change anything — every other rank
just idles. Three TODOs to make them help: split the data across the ranks,
exchange data, `merge()`. Worth knowing:

- The input is a **flat array of float32**. `MPI_File_read_at` takes its
  offset in **bytes**, so multiply by `sizeof(float)` — and do the multiply in
  `MPI_Offset` so a large `n` cannot overflow an `int` (the provided read
  already does this; keep it that way when you change the split).
- With a block distribution **every rank can compute its partner's element
  count itself**, so the merge never has to send a length ahead of the data
  (one less round trip per round).
- After merging, `std::swap(buf, tmp)` — swap pointers rather than copying
  back.
- Test with **power-of-two** process counts (1, 2, 4, 8, 16 …) — with anything
  else the tree comes out lopsided and the timing is not comparable.

### Testing locally

No cluster and no testcases needed:

```bash
python gen_testcase.py gen 1000 t01     # t01.in / t01.out / t01.txt
mpirun -n 4 ./parallel_merge_sort 1000 t01.in t01.result
python gen_testcase.py check t01.result t01.out
```

### Testing with the camp's testcases

Those come as `.in` (input), `.txt` (n / nodes / time limit) and `.out`
(expected output); inspect any of them with `od -t f4 <file>`. Point
`TESTCASE_DIR` at wherever they live:

```bash
TESTCASE_DIR=/path/to/testcases CASE=01 sbatch job.sh
```

`job.sh` reads `n` and the process count (the `nodes` field) out of the
`.txt`, runs, and verifies the output with `gen_testcase.py check`. Pass
`NP=<n>` to override the process count. The process count must fit the
allocation — bump `#SBATCH -n` in the script if a testcase asks for more.

Grading and the scoreboard run through the camp's own judge tool, from the lab
directory.

---

Everything here is self-contained: no path outside this repository is baked in
anywhere. The Slurm account and partition in each `job.sh` still need to match
your cluster, and testcase locations come in through `TESTCASE_DIR`; the C++
sources take everything on the command line.
