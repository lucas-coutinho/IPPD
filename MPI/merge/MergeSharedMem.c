#include<omp.h>
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>

void mergeSort(int vetor[], int comeco, int fim);
void merge(int vetor[], int comeco, int meio, int fim);
char* is_ordered(int *vec, int length);

int main(int argc, char** argv)
{
	int MAX_SIZE = atoi(argv[1]);
	int *unorderedVec, i;
	double start, end;
	unorderedVec = (int *) malloc(MAX_SIZE*sizeof(int));
	assert(unorderedVec != NULL);
	
	for(i = 0; i < MAX_SIZE; i++)
		unorderedVec[i] = rand() % MAX_SIZE;
	
	start = omp_get_wtime();
	#pragma omp parallel
	{	
		#pragma omp single
		mergeSort(unorderedVec, 0, MAX_SIZE - 1);
	}
	end = omp_get_wtime();
	printf("%s\nElapsed in %lf seconds \n", is_ordered(unorderedVec, MAX_SIZE), end - start);
	
	//for(i = 0; i < MAX_SIZE; i++) printf("%d ", unorderedVec[i]);
	//Sprintf("\n");
	
	return 0;
}

char* is_ordered(int *vec, int length)
{
	for(int i = 0; i < length - 1; i++)
	{
		if(vec[i] <= vec[i+1]){;}
		else return "Failed";
	}
	return "Success";
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
		
		#pragma omp task
        mergeSort(vetor, comeco, meio);
		#pragma omp task
        mergeSort(vetor, meio+1, fim);
		#pragma omp taskwait
        merge(vetor, comeco, meio, fim);
    }
}
