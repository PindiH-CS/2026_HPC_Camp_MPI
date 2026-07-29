// ============================================================================
// Lab 1 : point-to-point communication between two processes   (TEMPLATE)
//
//   compile and run mpi_send_recv.cc
//   change it so that process 0 sends "Hello, Process 1" to process 1
//
// Build & run:
//   mpicxx -o mpi_send_recv mpi_send_recv.cc
//   mpirun -np 2 ./mpi_send_recv
//
//   MPI_Send(buf, count, type, dest,   tag, comm)
//   MPI_Recv(buf, count, type, source, tag, comm, status)
//     buf    pointer to the data to send / the buffer to receive into
//     count  number of elements (on recv, the buffer's capacity)
//     type   element type, e.g. MPI_CHAR
//     dest   / source   the other process's rank
//     tag    must match on both sides
//     comm   communicator (MPI_COMM_WORLD)
//     status use MPI_STATUS_IGNORE if you don't need it
// ============================================================================
#include <mpi.h>

#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) fprintf(stderr, "this example needs at least 2 processes\n");
        MPI_Finalize();
        return 1;
    }

    const int TAG = 0;
    char message[64];

    if (rank == 0) {
        // TODO: write "Hello, Process 1" into message, then MPI_Send it to
        //       rank 1. Watch the count -- the string's terminating '\0' has
        //       to travel too, or the receiver won't know where it ends.

    } else if (rank == 1) {
        // TODO: MPI_Recv the message from rank 0 and print it.

    }

    MPI_Finalize();
    return 0;
}
