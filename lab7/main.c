#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int counter = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* _write(){
	int i = 0;
	while (i < 5) {
		pthread_rwlock_wrlock(&rwlock);
		counter += 1;
		pthread_rwlock_unlock(&rwlock);
		i += 1;
		sleep(1);
	}
}

void* _read(){
	int i = 0;
	while (i < 5) {
		pthread_rwlock_rdlock(&rwlock);
		printf("TID = %x\t\t||\tCOUNTER = %d\n", pthread_self(), counter);
		pthread_rwlock_unlock(&rwlock);
		i++;
		sleep(1);
	}
	pthread_exit(NULL);
}

int main(){
	pthread_t thread_w;
	pthread_t array_thread_r[10];
	
	pthread_create(&thread_w, NULL, _write, NULL);
	for(int i=0; i < 10; i++)
		pthread_create(&array_thread_r[i], NULL, _read, NULL);
	
	pthread_join(thread_w, NULL);
	for(int i=0; i < 10; i++)
		pthread_join(array_thread_r[i], NULL);
	
	pthread_rwlock_destroy(&rwlock);

	return 0;
}
