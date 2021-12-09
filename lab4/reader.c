#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MEM_NAME "writer.c"


struct sembuf sem_lock = {0, -1, 1}, sem_open = {0, 1, 0};

typedef struct data_struct {
	time_t time;
	pid_t pid;
} d_struct;


char* timeToChar(int num){
	char* time = (char*) malloc(2);
	if (num < 10){
		time[0] = '0';
		time[1] = num + '0';
		return time;
	}
	sprintf(time, "%d", num);
	return time;
}

int main(int argc, char** argv){
	
	//while (1){

		key_t key = ftok(MEM_NAME, 1);
		if (key < 0){
			printf("[READER] ftok() failed with code: %d\n", errno);
			return -1;
		}

		int shmem_id = shmget(key, sizeof(d_struct), 0666);
		if (shmem_id < 0) {
			printf("ERROR!! shmget() failed with code: %d\n\n", errno);
			return -1;
		}

		int sem_id = semget(key, 1, IPC_CREAT | 0666);
		if (sem_id < 0) {
			printf("ERROR!! semget() failed with code: %d\n\n", errno);
			return -1;
		}


		void* ptr_to_shm = shmat(shmem_id, NULL, 0);
		if (ptr_to_shm == (void*)-1){
			printf("ERROR!! shmat() failed with code: %d\n\n", errno);
			return -1;
		}
		
		semop(sem_id, &sem_lock, 1);
		d_struct g_data = *((d_struct*)ptr_to_shm);
		semop(sem_id, &sem_open, 1);
		
		int p_hh = localtime(&g_data.time)->tm_hour;
		int p_mm = localtime(&g_data.time)->tm_min;
		int p_ss = localtime(&g_data.time)->tm_sec;
			
		time_t ch_time = time(NULL);
		int hh = localtime(&ch_time)->tm_hour;
		int mm = localtime(&ch_time)->tm_min;
		int ss = localtime(&ch_time)->tm_sec;
			
		printf("----\nPARENT PID = %d\n", g_data.pid);
		printf("PARENT TIME = %s::%s::%s\n", timeToChar(p_hh), timeToChar(p_mm), timeToChar(p_ss));
		printf("CHILD TIME = %s::%s::%s\n\n", timeToChar(hh), timeToChar(mm), timeToChar(ss));

				
		shmdt(ptr_to_shm);
		//shmctl(shmem_id, IPC_RMID, NULL);
				
	//}	

	return 0;
}
