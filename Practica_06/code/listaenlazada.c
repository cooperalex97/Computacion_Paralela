#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

double timeval_diff(struct timeval *a, struct timeval *b)
{
  return (double)(a->tv_sec + (double)a->tv_usec/1000000) - (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
struct list_node_s 
{
	int data;
	struct list_node_s* next;
	pthread_mutex_t mutex;
};

int thread_count;
struct list_node_s **head_p;
pthread_mutex_t head_p_mutex;
pthread_mutex_t mutex;

int Member(int value) 
{
	struct list_node_s* temp_p;
	pthread_mutex_lock(&head_p_mutex);
	temp_p = *head_p;
	while (temp_p != NULL && temp_p->data < value) 
	{
		if (temp_p->next != NULL)
			pthread_mutex_lock(&(temp_p->next->mutex));
		if (temp_p == *head_p)
			pthread_mutex_unlock(&head_p_mutex);
		pthread_mutex_unlock(&(temp_p->mutex));
		temp_p = temp_p->next;
	}
	if (temp_p == NULL || temp_p->data > value) 
	{
		if (temp_p == *head_p)
			pthread_mutex_unlock(&head_p_mutex);
		if (temp_p != NULL)
			pthread_mutex_unlock(&(temp_p->mutex));
		return 0;
	}
	else 
	{
		if (temp_p == *head_p)
			pthread_mutex_unlock(&head_p_mutex);
		pthread_mutex_unlock(&(temp_p->mutex));
		return 1;
	}
}

int Insert(int value) 
{
	struct list_node_s* curr_p = *head_p;
	struct list_node_s* pred_p = NULL;
	struct list_node_s* temp_p;
	while (curr_p != NULL && curr_p->data < value) 
	{
		pred_p = curr_p;
		curr_p = curr_p->next;
	}
	if (curr_p == NULL || curr_p->data > value)
	{
		temp_p = malloc(sizeof(struct list_node_s));
		temp_p->data = value;
		temp_p->next = curr_p;
		if (pred_p == NULL)
			*head_p = temp_p;
		else
			pred_p->next = temp_p;
		return 1;
	} 
	else return 0;
}

int Delete(int value) 
{
	struct list_node_s* curr_p = *head_p;
	struct list_node_s* pred_p = NULL;
	while (curr_p != NULL && curr_p->data < value) 
	{
		pred_p = curr_p;
		curr_p = curr_p->next;
	}
	if (curr_p != NULL && curr_p->data == value) 
	{
		if (pred_p == NULL) 
		{ 
			*head_p = curr_p->next;
			free(curr_p);
		} 
		else 
		{
			pred_p->next = curr_p->next;
			free(curr_p);
		}
		return 1;
	} 
	else return 0;
}

void* Probar(void * rank)
{
	long my_rank=(long) rank;
	pthread_mutex_lock(&mutex);
	Insert((int)my_rank);
	pthread_mutex_unlock(&mutex);
	int test=Member((int) my_rank);
	printf("Numero es %d\n", test);
	return NULL;
}

int main(int argc,char* argv[])
{

	long thread;
	pthread_t* thread_handles;
	struct list_node_s* list;
	list=malloc(sizeof(struct list_node_s));
	list->data=0;
	list->next=NULL;
	head_p=&list;
	thread_count=strtol(argv[1],NULL,10);
	thread_handles=malloc (thread_count*sizeof(pthread_t));
	struct timeval t_ini, t_fin;
  	double secs;
  	gettimeofday(&t_ini, NULL);
	for(thread=0;thread<thread_count;thread++)
	{
		pthread_create(&thread_handles[thread],NULL,Probar,(void *)thread);
	}
	for(thread=0;thread<thread_count;thread++)
	{
		pthread_join(thread_handles[thread],NULL);
	}
	gettimeofday(&t_fin, NULL);
  	secs = timeval_diff(&t_fin, &t_ini);
  	printf("%.16g milliseconds\n", secs * 1000.0);
	free(thread_handles);
	return 0;
}
