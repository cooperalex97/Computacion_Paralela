#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define BARRIER_COUNT 750
int thread_count;
int barrier_thread_counts[BARRIER_COUNT];
pthread_barrier_t barrier_p;

void Usage(char* prog_name);
void *Thread_work(void* rank);


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
   pthread_barrier_init(&barrier_p,NULL,thread_count);

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          Thread_work, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);

   pthread_barrier_destroy(&barrier_p);
   free(thread_handles);
   return 0;
}  /* main */



void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
}  /* Usage */



void *Thread_work(void* rank) {
#  ifdef DEBUG
   long my_rank = (long) rank; 
#  endif
   int i;

   for (i = 0; i < BARRIER_COUNT; i++) {
      barrier_thread_counts[i]++;
      pthread_barrier_wait(&barrier_p);
   }

   return NULL;
}  /* Thread_work */
