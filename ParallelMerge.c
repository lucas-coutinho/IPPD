#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void merge(int vetor[], int comeco, int meio, int fim) ;
void mergeSort(int vetor[], int comeco, int fim);

int main(int argc, char** argv)
{
	int MAX_SIZE = atoi(argv[1]);
	FILE *fp;
	double sort_time_stamp, clone_data_time_stamp;
	clock_t t_start, t_end;
	int *unorderedVec = (int *) malloc(MAX_SIZE*sizeof(int));
	int i;
	
	for(i = 0; i < MAX_SIZE; i++)
		unorderedVec[i] = rand() % MAX_SIZE;

	
	MPI_Init(NULL,NULL);
	
	int world_size; //Get the number of process
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int world_rank; // Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int size;
	int *recv_data;
	recv_data = (int *) malloc((MAX_SIZE/world_size)*sizeof(int));
	
	t_start = clock();
	MPI_Scatter(
    (void*) unorderedVec, //Unordered array 
    (MAX_SIZE/world_size), // Chunck_size
    MPI_INT, //INT
    (void*) recv_data, //
    (MAX_SIZE/world_size), //Chunck_size
    MPI_INT,//INT
    0, //0
    MPI_COMM_WORLD); // MPI_COMM_WORLD)
	t_end = clock();
	clone_data_time_stamp = (double) t_end - t_start;
	
	
	size = MAX_SIZE/world_size;
	
	t_start = clock();
	
	mergeSort(recv_data, 0, size - 1);
	
	int *orderedVec = NULL;
	
	if (world_rank == 0) {
		orderedVec = (int *)  malloc(sizeof(int) * MAX_SIZE);
	}
	MPI_Gather(
    recv_data,
    size,
    MPI_INT,
    (void*) orderedVec,
    size,
    MPI_INT,
    0,
    MPI_COMM_WORLD);
	
	if(world_rank == 0)
	{
		mergeSort( orderedVec, 0, MAX_SIZE -1 );
		t_end = clock();
		sort_time_stamp = (double) t_end - t_start;
		if((fp = fopen("TimeMeasure.txt", "w")) == NULL)
		{
			printf("Nao conseguiu abrir o arquivo!\n");
			exit(1);
		}
		fprintf(fp,"Cloning Data time: %lf\n", clone_data_time_stamp/CLOCKS_PER_SEC);
		fprintf(fp,"Sorting time: %lf\n", sort_time_stamp/CLOCKS_PER_SEC);
		fflush(fp);
		fclose(fp);
		for(i = 0; i < MAX_SIZE; i++) printf("%d ", orderedVec[i]);
		printf("\n");
	}
	
	
	free(orderedVec);
	free(recv_data);
	free(unorderedVec);
	MPI_Finalize();
	
	
	return 0;
	
}
void merge(int vetor[], int comeco, int meio, int fim) {
    int com1 = comeco, com2 = meio+1, comAux = 0, tam = fim-comeco+1;
    int *vetAux;
    vetAux = (int*)malloc(tam * sizeof(int));

    while(com1 <= meio && com2 <= fim){
        if(vetor[com1] < vetor[com2]) {
            vetAux[comAux] = vetor[com1];
            com1++;
        } else {
            vetAux[comAux] = vetor[com2];
            com2++;
        }
        comAux++;
    }

    while(com1 <= meio){  //Caso ainda haja elementos na primeira metade
        vetAux[comAux] = vetor[com1];
        comAux++;
        com1++;
    }

    while(com2 <= fim) {   //Caso ainda haja elementos na segunda metade
        vetAux[comAux] = vetor[com2];
        comAux++;
        com2++;
    }

    for(comAux = comeco; comAux <= fim; comAux++){    //Move os elementos de volta para o vetor original
        vetor[comAux] = vetAux[comAux-comeco];
    }
    
    free(vetAux);
}

void mergeSort(int vetor[], int comeco, int fim){
    if (comeco < fim) {
        int meio = (fim+comeco)/2;

        mergeSort(vetor, comeco, meio);
        mergeSort(vetor, meio+1, fim);
        merge(vetor, comeco, meio, fim);
    }
}
