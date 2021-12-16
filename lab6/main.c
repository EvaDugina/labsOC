#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t condition;

void* _write(){
	while (counter < 5) {
		pthread_mutex_lock(&mutex);
		counter += 1;
		pthread_mutex_unlock(&mutex);
		pthread_cond_broadcast(&condition);
		sleep(1);
	}
}

void* _read(){
	int i = 0;
	while (i < 5) {
		pthread_mutex_lock(&mutex);		
		pthread_cond_wait(&condition, &mutex);
		printf("TID = %x\t\t||\tCOUNTER = %d\n", pthread_self(), counter);
		pthread_mutex_unlock(&mutex);
		i++;
		//sleep(1);
	}
	pthread_exit(NULL);
}

int main(){
	pthread_t thread_w;
	pthread_t array_thread_r[10];
	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
	
	for(int i=0; i < 10; i++)
		pthread_create(&array_thread_r[i], NULL, _read, NULL);
	pthread_create(&thread_w, NULL, _write, NULL);
	
	for(int i=0; i < 10; i++)
		pthread_join(array_thread_r[i], NULL);
	pthread_mutex_destroy(&mutex);

	pthread_join(thread_w, NULL);
	pthread_cond_destroy(&condition);

	return 0;
}
