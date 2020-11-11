#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RAND_RANGE_NUM 100

int * getZeroVector(int size){
	int * res = (int*) malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++){
		res[i] = 0;
	}
	return res;
}

int ** getZeroMatrix(int fil, int col){
	int ** res = (int**) malloc(sizeof(void*) * fil);
	for(int i = 0; i < fil; i++){
		res[fil] = (int*) malloc(sizeof(int) * col);
	}
	for(int i = 0; i < fil; i++){
		for(int j = 0; j < col; j++){
			res[i][j] = 0;
		}
	}
	return res;
}

void printMatrix(int ** matrix, int fil, int col){
	for(int i = 0; i < fil; i++){
		for(int j = 0; j < col; j++){
			printf("%d ",matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void printVectorMatrix(int * matrix, int fil, int col){
	for(int i = 0; i < fil; i++){
		for(int j = 0; j < col; j++){
			printf("%d ", matrix[i * col + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void printVector(int * vector, int size){
	for(int i = 0; i < size; i++){
		printf("%d ", vector[i]);
	}
	printf("\n");
	printf("\n");
}

int ** getRandomMatrix(int fil, int col){
	srand(time(NULL));
	int ** res = (int **) malloc(sizeof(void*) * fil);
	for(int i = 0; i < fil; i++){
		size_t tam = sizeof(int) * col;
		res[i] = (int *) malloc(tam);
	}
	for(int i = 0; i < fil; i++){
		for(int j = 0; j < col; j++){
			res[i][j] = rand() % RAND_RANGE_NUM;
		}
	}
	return res;
}

int * getRandomVector(int size){
	srand(time(NULL));
	int * res = (int *) malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++){
		res[i] = rand() % RAND_RANGE_NUM;
	}
	return res;
}

#endif