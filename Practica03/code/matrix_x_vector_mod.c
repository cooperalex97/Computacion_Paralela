#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "utils.h"
#include "timer.h"

int * vector;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
	int local_fil;
	int colum;
	int ** local_matrix;
} MethodArgs;

void * matrixVectorMultiplication(void * args){
	MethodArgs m_args = *((MethodArgs*) args);
	int * res = (int *) malloc(sizeof(int) * m_args.local_fil);
	for(int i = 0; i < m_args.local_fil; i++){
		res[i] = 0;
		for(int j = 0; j < m_args.colum; j++){
			res[i] += m_args.local_matrix[i][j] * vector[j];
		}
	}
	return (void *) res;
}

int main(int argv, char ** argc){
	if(argv != 4){
		printf("Faltan Argumentos <numThreads> <tamMatrixFil> <tamMatrixCol>\n");
		return 0;
	}
	double ini, end;
	int numThreads = atoi(argc[1]);
	int tamMatrixFil = atoi(argc[2]);
	int tamMatrixCol = atoi(argc[3]);

	int ** matrix = getRandomMatrix(tamMatrixFil, tamMatrixCol);
	int local_fil = tamMatrixFil / numThreads;
	vector = getRandomVector(tamMatrixCol);
	pthread_t threads[numThreads];
	MethodArgs threads_args[numThreads];
	int pedazos = 10;
	int memSize = tamMatrixCol / pedazos;

	GET_TIME(ini);
	for(int i = 0; i < numThreads; i++){
		threads_args[i].local_matrix = (int **) malloc(sizeof(void*) * local_fil);
		for(int j = 0; j < local_fil; j++){
			size_t tam = sizeof(int) * tamMatrixCol;
			threads_args[i].local_matrix[j] = (int *) malloc(tam);
			
			for(int k = 0; k < pedazos; k++){
				memcpy((void*) &(threads_args[i].local_matrix[j][k * pedazos]),
						(void*) &(matrix[i*local_fil + j][k * pedazos]),
							memSize * sizeof(int));
			}
			
			
		}
		threads_args[i].local_fil = local_fil;
		threads_args[i].colum = tamMatrixCol;
		pthread_create(&threads[i],NULL,matrixVectorMultiplication,(void*) &threads_args[i]);
	}
	int * res = malloc(sizeof(int) * tamMatrixFil);
	for(int i = 0; i < numThreads; i++){
		int * temp = malloc(sizeof(int) * local_fil);
		pthread_join(threads[i],(void **) &temp);
		memcpy(&res[i*local_fil],temp,sizeof(int) * local_fil);
	}
	GET_TIME(end);
	double seconds = end - ini;
	
	GET_TIME(ini);
	for(int i = 0; i < 1; i++){
		threads_args[i].local_matrix = (int **) malloc(sizeof(void*) * local_fil);
		for(int j = 0; j < local_fil; j++){
			size_t tam = sizeof(int) * tamMatrixCol;
			threads_args[i].local_matrix[j] = (int *) malloc(tam);
			
			for(int k = 0; k < pedazos; k++){
				memcpy((void*) &(threads_args[i].local_matrix[j][k * pedazos]),
						(void*) &(matrix[i*local_fil + j][k * pedazos]),
							memSize * sizeof(int));
			}
			
			
		}
		threads_args[i].local_fil = local_fil;
		threads_args[i].colum = tamMatrixCol;
		pthread_create(&threads[i],NULL,matrixVectorMultiplication,(void*) &threads_args[i]);
	}
	int * res2 = malloc(sizeof(int) * tamMatrixFil);
	for(int i = 0; i < 1; i++){
		int * temp = malloc(sizeof(int) * local_fil);
		pthread_join(threads[i],(void **) &temp);
		memcpy(&res2[i*local_fil],temp,sizeof(int) * local_fil);
	}
	GET_TIME(end);
	double seconds2 = end - ini;
	printf("Tiempo->%lf\n", seconds);
	printf("Eficiencia->%lf\n", seconds2/seconds);
	//printVector(res,tamMatrixFil);
}