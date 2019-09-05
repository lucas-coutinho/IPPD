#include <stdio.h>
#include <mpi.h>

#define PING_PONG_MAX 10

int main()
{
	MPI_Init(NULL,NULL);
	
	int world_size; //Get the number of process
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int world_rank; // Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	int ping_pong_count = 0;
	int partner_rank = (world_rank + 1) % 2;
	
	while(ping_pong_count < PING_PONG_MAX )
	{
		if(world_rank == ping_pong_count % 2)
		{ 
			ping_pong_count++;
			MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
			printf("%d sent and incremented ping_pong_count "
               "%d to %d\n", world_rank, ping_pong_count,
               partner_rank);
		}
		else
		{
			
			MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0,	MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("%d received ping_pong_count %d from %d\n", world_rank, ping_pong_count, partner_rank);
		}
	}
	
	MPI_Finalize();
	return 0;
}