#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Get_dims(int* m_p);
void Read_matrix(char prompt[], double A[], int m, int n);
void Read_vector(char prompt [], double x[], int n);
void Print_matrix(char title[], double A[], int m, int n);
void Print_vector(char title[], double y[], int m);
void Mat_vect_mult(double A[], double x[], double y[], int m, int n);

int main(void) {
   float t0,t1, tiempo;
   double* A = NULL;
   double* x = NULL;
   double* y = NULL;
   int m=512;
   for (int i = 0; i < 5; ++i) {
     m = m*2;
     A = malloc(m*m*sizeof(double));
     x = malloc(m*sizeof(double));
     y = malloc(m*sizeof(double));
     if (A == NULL || x == NULL || y == NULL) {
        fprintf(stderr, "Can't allocate storage\n");
        exit(-1);
     }
     Read_matrix("A", A, m, m);
     Read_vector("x", x, m);
    t0 = clock();
    Mat_vect_mult(A, x, y, m, m);
    t1 = clock();
    tiempo = ((t1-t0)/CLOCKS_PER_SEC);
    printf("%f\n",tiempo);
     free(A);free(x);free(y);
   }
   return 0;
}

void Get_dims(int* m_p) {
   printf("Enter the number of rows and column\n");
   scanf("%d", m_p);

   if (*m_p <= 0) {
      fprintf(stderr, "m and n must be positive\n");
      exit(-1);
   }
}

void Read_matrix(char prompt[], double A[], int m, int n) {
   int i, j;
   //printf("Enter the matrix %s\n", prompt);
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
        A[i*n+j] = ((double) random())/((double) RAND_MAX);
         // scanf("%lf", &A[i*n+j]);
}

void Read_vector(char prompt[], double x[], int n) 
{
   int i;

   //printf("Enter the vector %s\n", prompt);
   for (i = 0; i < n; i++)
      x[i] = ((double) random())/((double) RAND_MAX);
      //scanf("%lf", &x[i]);
} 

void Print_matrix(char title[],double  A[],int m, int n) {
   int i, j;
   printf("\nThe matrix %s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%f ", A[i*n+j]);
      printf("\n");
   }
}  

void Print_vector(char title[], double y[], int m) {
   int i;

   printf("\nThe vector %s\n", title);
   for (i = 0; i < m; i++)
      printf("%f ", y[i]);
   printf("\n");
}


void Mat_vect_mult(double A[], double x[], double y[], int m, int n) {
   int i, j;
   for (i = 0; i < m; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
         y[i] += A[i*n+j]*x[j];
   }
}