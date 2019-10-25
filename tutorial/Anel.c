#include <stdio.h>
#include <mpi.h>

//#define PING_PONG_MAX 10

int main()
{
	MPI_Init(NULL,NULL);
	
	int world_size; //Get the number of process
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int world_rank; // Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	

	int ring;
	
	if(world_rank != 0)
	{
		MPI_Recv(&ring, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("%d received ring %d from %d\n", world_rank, ring, world_rank - 1);
	}
	else
	{
		ring = -1;
	}
	
	MPI_Send(&ring, 1, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
	printf("%d sending ring %d to %d\n", world_rank, ring, (world_rank + 1) % world_size);
	
	if(world_rank == 0)
	{
		MPI_Recv(&ring, 1, MPI_INT, world_size - 1,	0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("%d received ring %d from %d\n", world_rank, ring, world_size - 1);
	}
	
	MPI_Finalize();
	return 0;
}