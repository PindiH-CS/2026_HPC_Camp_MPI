// ============================================================================
// Lab 2 : Monte Carlo Pi calculation   (single-process starting point)
//
//   Scatter random points over the square [-r, r] x [-r, r]. The probability
//   that a point lands inside the inscribed circle is
//
//        P = (circle area) / (square area) = (pi * r^2) / (2r)^2 = pi / 4
//
//   => pi ~= 4 * (points inside the circle) / (total points)
//
//   Taking x, y in [0, 1] (one quadrant) is enough -- the ratio is the same.
//
// This version runs on ONE process and already works:
//   mpicxx -o pi_calc pi_calc.cc
//   ./pi_calc 1000000
//
// The variables are already split into local_* (what THIS process does) and
// total_* (everyone together) -- with one process, local is simply total.
// YOUR TASK: fill in the MPI so "everyone together" means something.
//   1. start MPI and get the real rank / size           (same calls as lab 1)
//   2. make local_tests this rank's fair share of total_tests
//   3. combine every rank's local_hits into total_hits on rank 0
//   4. only rank 0 prints; shut MPI down at every exit
// ============================================================================
#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) {
    // ------------------------------------------------------------------
    // TODO 1: start MPI.
    //   #include <mpi.h> at the top, then the same calls as lab 1:
    //   MPI_Init(&argc, &argv), and fill rank / size in with
    //   MPI_Comm_rank / MPI_Comm_size instead of pretending to be alone.
    //   (Remember MPI_Finalize at EVERY exit, including the usage error
    //   just below.)
    // ------------------------------------------------------------------
    int rank = 0;  // who am I?
    int size = 1;  // how many of us are there?

    if (argc != 2) {
        if (rank == 0) fprintf(stderr, "usage: %s <# of tests>\n", argv[0]);
        return 1;
    }

    int total_tests = atoi(argv[1]);

    // ------------------------------------------------------------------
    // TODO 2: split the work.
    //   With size == 1 this rank runs everything, so the line below is
    //   already correct. With more processes it is only almost correct:
    //   the local counts of all ranks must add up to EXACTLY
    //   total_tests. Who runs the remainder when it does not divide
    //   evenly?
    // ------------------------------------------------------------------
    int local_tests = total_tests / size;

    unsigned int seed = (unsigned int)(12345 + rank * 7919);
    int local_hits = 0;

    for (int i = 0; i < local_tests; i++) {
        double x = (double)rand_r(&seed) / (double)RAND_MAX;  // [0, 1]
        double y = (double)rand_r(&seed) / (double)RAND_MAX;  // [0, 1]
        if (x * x + y * y <= 1.0) local_hits++;  // inside the quarter circle
    }

    // ------------------------------------------------------------------
    // TODO 3: gather the results.
    //   the count of the single process IS the total. With more processes, sum every
    //   rank's local_hits into total_hits on rank 0
    // ------------------------------------------------------------------
    int total_hits = local_hits;

    // ------------------------------------------------------------------
    // TODO 4: after the reduce only rank 0 holds the full count, so the
    //   `if (rank == 0)` below is already what you want -- just make
    //   sure the program ends with MPI_Finalize().
    // ------------------------------------------------------------------
    if (rank == 0) {
        double pi = 4.0 * (double)total_hits / (double)total_tests;
        printf("tests = %d\n", total_tests);
        printf("pi    = %.10f\n", pi);
    }
    return 0;
}
