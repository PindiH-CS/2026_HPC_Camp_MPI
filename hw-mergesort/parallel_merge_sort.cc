// ============================================================================
// Homework : parallel merge sort with MPI   (TEMPLATE)
//
//   Input : a flat array of single-precision floats, read with MPI-IO
//   Output: the same floats in ascending order, written with MPI-IO
//
// Usage:  mpirun -n <# of processes> ./parallel_merge_sort <n> <input> <output>
//         (n = number of elements, listed in the testcase's .txt file)
//
// Algorithm :
//
//   step 1 : every rank reads its own slice and sorts it locally
//   step 2 : merge pairwise; each round halves the number of live ranks
//
//        P0 P1 P2 P3 P4 P5 P6 P7      <- local sort
//         \ /   \ /   \ /   \ /
//         P0    P2    P4    P6        <- step = 1
//          \   /       \   /
//           P0          P4            <- step = 2
//             \        /
//                 P0                  <- step = 4, everything is on rank 0
//
//   step 3 : rank 0 writes the result out
//
// TODO:
//   1. Split the data across the processes (change local_n / local_off)
//   2. Implement the data exchange (send / recv)
//   3. Implement the merge function
//   4. Tune the code / the environment and make it faster
//      (test with power-of-two process counts: 1, 2, 4, 8, 16 ...)
// ============================================================================
#include <mpi.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
// Block distribution helpers:
//   the first (n % p) ranks get n/p + 1 elements, the rest get n/p.
// ---------------------------------------------------------------------------
static inline int offset_of(int r, int n, int p) {
    if (r >= p) return n;
    return r * (n / p) + std::min(r, n % p);
}
static inline int count_of(int r, int n, int p) {
    return offset_of(r + 1, n, p) - offset_of(r, n, p);
}

// ---------------------------------------------------------------------------
// TODO 3: the merge function.
//   Merge the two *already sorted* arrays a[0..na) and b[0..nb) into
//   out[0..na+nb).
// ---------------------------------------------------------------------------
static void merge(const float *a, int na, const float *b, int nb, float *out) {
    // <-- implement me
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 4) {
        if (rank == 0)
            fprintf(stderr, "usage: %s <n> <input file> <output file>\n",
                    argv[0]);
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);
    const char *in_file = argv[2];
    const char *out_file = argv[3];

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // ------------------------------------------------------------------
    // TODO 1: split the data across the processes.
    //   Right now rank 0 takes the whole array and everyone else takes
    //   nothing -- that is the sequential version. Change local_n and
    //   local_off so every rank gets its own slice; offset_of / count_of
    //   above do the math for you.
    // ------------------------------------------------------------------
    int local_n = (rank == 0) ? n : 0;  // <-- change me
    int local_off = 0;                  // <-- change me


    int capacity = local_n;
    for (int step = 1; step < size; step *= 2) {
        if (rank % (2 * step) != 0) break;
        capacity = offset_of(std::min(rank + 2 * step, size), n, size) -
                   offset_of(rank, n, size);
    }

    float *buf = (float *)malloc(sizeof(float) * (capacity > 0 ? capacity : 1));
    float *tmp = (float *)malloc(sizeof(float) * (capacity > 0 ? capacity : 1));

    MPI_File fin;
    MPI_File_open(MPI_COMM_WORLD, in_file, MPI_MODE_RDONLY, MPI_INFO_NULL,
                  &fin);
    MPI_File_read_at(fin, (MPI_Offset)local_off * sizeof(float), buf, local_n,
                     MPI_FLOAT, MPI_STATUS_IGNORE);
    MPI_File_close(&fin);

    // ------------------------------------------------------------------
    // local sort
    //   You can write your own merge sort, or just call std::sort
    //   (introsort, usually faster).
    // ------------------------------------------------------------------
    std::sort(buf, buf + local_n);

    // ------------------------------------------------------------------
    // TODO 2: the merge tree.
    //   for (step = 1; step < size; step *= 2)
    //       if (rank % (2*step) != 0)      -> I am the sender this round
    //             send local_n floats from buf to rank - step, then break
    //             out of the loop (the later rounds do not involve me)
    //       else                            -> I am the receiver
    //             partner = rank + step;  (nothing to do if partner >= size)
    //             receive the partner's data just past my own,
    //             merge(buf, local_n, recv, recv_n, tmp);
    //             swap(buf, tmp);  local_n += recv_n;
    //   How do you know how much the partner will send?
    //         (a) compute it with offset_of / count_of -- no extra message
    //         (b) MPI_Probe + MPI_Get_count to ask
    // ------------------------------------------------------------------

    // <-- implement the merge tree here

    // ------------------------------------------------------------------
    // Output: after the merge tree, rank 0 holds the whole answer in
    // buf[0..local_n), so MPI_COMM_SELF is enough here.
    // ------------------------------------------------------------------
    if (rank == 0) {
        MPI_File fout;
        MPI_File_open(MPI_COMM_SELF, out_file,
                      MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fout);
        MPI_File_write_at(fout, 0, buf, local_n, MPI_FLOAT,
                          MPI_STATUS_IGNORE);
        MPI_File_close(&fout);
    }

    double t1 = MPI_Wtime();
    if (rank == 0) fprintf(stderr, "elapsed = %.6f s\n", t1 - t0);

    free(buf);
    free(tmp);
    MPI_Finalize();
    return 0;
}
