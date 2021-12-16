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
#include <sys/stat.h>
#include <sys/wait.h>

#define MEM_NAME "writer.c"


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

	sleep(1);

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

	void* ptr_to_shm = shmat(shmem_id, NULL, 0);
	if (ptr_to_shm == (void*)-1){
		printf("ERROR!! shmat() failed with code: %d\n\n", errno);
		return -1;
	}
			
	sleep(2);
		
	d_struct g_data = *((d_struct*)ptr_to_shm);
			
	time_t ch_time = time(NULL);
			
	printf("----\nPARENT PID = %d\n", g_data.pid);
	printf("PARENT TIME = %s", ctime(&g_data.time));
	printf("CHILD TIME = %s", ctime(&ch_time));

				
	shmdt(ptr_to_shm);

	return 0;
}
