#include<mpi.h>

void sendint(int num, int dest, int tag)
{
	MPI_Send(&num, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
}

MPI_Status status;

void recvint(int num, int sour, int tag){
	MPI_Recv(&num, 1, MPI_INT, sour, tag, MPI_COMM_WORLD, &status);
}
