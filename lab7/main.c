#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int counter = 0;
pthread_mutex_t mutex;

void* _write(){
	int i = 0;
	while (i < 5) {
		pthread_mutex_lock(&mutex);
		counter += 1;
		pthread_mutex_unlock(&mutex);
		sleep(1);
		i += 1;
	}
}

void* _read(){
	while (1) {
		pthread_mutex_lock(&mutex);
		printf("TID = %x\t\t||\tCOUNTER = %d\n", pthread_self(), counter);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}

int main(){
	pthread_t thread_w;
	pthread_t array_thread_r[10];
	
	pthread_mutex_init(&mutex, NULL);

	pthread_create(&thread_w, NULL, _write, NULL);
	for(int i=0; i < 10; i++)
		pthread_create(&array_thread_r[i], NULL, _read, NULL);
	
	pthread_join(thread_w, NULL);
	for(int i=0; i < 10; i++)
		pthread_join(array_thread_r[i], NULL);
	
	pthread_mutex_destroy(&mutex);

	return 0;
}
