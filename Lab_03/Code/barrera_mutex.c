/* File:  
 *    barrera_mutex.c
 *
 * Proposito:
 *   Utilizar barreras de espera ocupada para sincronizar hilos.
 *
 * Input:
 *    none
 * Output:
 *    Tiempo para barreras BARRIER_COUNT
 *
 * Compilar:
 *    gcc -g -Wall -o barreram barrera_mutex.c -lpthread
 *    mpicc -o barreram barrera_mutex.c -lpthread
 *
 * Uso:
 *    ./barreram <numero de hilos>
 *    mpirun -np 1 ./barreram <numero de hilos>
 *
 * Nota:
 *    Compilar flag DEBUG imprimira un mensaje despues de cada barrera   
 *
 * IPP:   Section 4.8.1 (p. 177)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define BARRIER_COUNT 750

int thread_count;
int barrier_thread_counts[BARRIER_COUNT];
pthread_mutex_t barrier_mutex;

void Usage(char* prog_name);
void *Thread_work(void* rank);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread, i;
   pthread_t* thread_handles; 
   double start, finish;

   if (argc != 2)
      Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);

   thread_handles = malloc (thread_count*sizeof(pthread_t));
   for (i = 0; i < BARRIER_COUNT; i++)
      barrier_thread_counts[i] = 0;
   pthread_mutex_init(&barrier_mutex, NULL);

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          Thread_work, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }
   GET_TIME(finish);
   printf("Tiempo transcurrido = %e segundos\n", finish - start);

   pthread_mutex_destroy(&barrier_mutex);
   free(thread_handles);
   return 0;
}  /* main */


/*--------------------------------------------------------------------
 * Funcion  : Usage
 * Proposito: Imprimir línea de comando para función y terminar
 * In arg   : prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <numero de hilos>\n", prog_name);
   exit(0);
}  /* Usage */


/*-------------------------------------------------------------------
 * Funcion:    Thread_work
 * Proposito:    Correr barreras BARRIER_COUNT 
 * In arg:      rank
 * Global var:  thread_count, barrier_thread_counts, barrier_mutex
 * Return val:  Ignorado
 */
void *Thread_work(void* rank) {
#  ifdef DEBUG
   long my_rank = (long) rank; 
#  endif
   int i;

   for (i = 0; i < BARRIER_COUNT; i++) {
      pthread_mutex_lock(&barrier_mutex);
      barrier_thread_counts[i]++;
      pthread_mutex_unlock(&barrier_mutex);
      while (barrier_thread_counts[i] < thread_count);
   }

   return NULL;
}  /* Thread_work */
