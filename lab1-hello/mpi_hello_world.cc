// ============================================================================
// Lab 1 : hello world with several processes
//
//   load and check your MPI environment
//   uncomment the MPI calls below so the program works
//   compile it:  mpicxx -o mpi_hello_world mpi_hello_world.cc
//   run it:      mpirun -np 4 ./mpi_hello_world

//   try different values of -np

//   Think about it: do the processes print in order?
// ============================================================================
#include <mpi.h>

#include <cstdio>

int main(int argc, char **argv) {
    // Every MPI program starts here. Before this call almost nothing in the
    // MPI library may be used.
    // MPI_Init(&argc, &argv);

    int rank = -1;  // who am I?                 (0 .. size-1)
    int size = -1;  // how many of us are there?

    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &size);

    // MPI_COMM_WORLD is the communicator holding every process that mpirun
    // started. A rank only has meaning relative to a communicator.

    // Which machine did this process land on? Interesting once you run
    // across nodes.
    char host[MPI_MAX_PROCESSOR_NAME] = "unknown";
    // int host_len;
    // MPI_Get_processor_name(host, &host_len);

    printf("Hello world from process %d of %d on %s\n", rank, size, host);

    // Releases everything MPI allocated. Nothing MPI may be called afterwards.
    // MPI_Finalize();
    return 0;
}
