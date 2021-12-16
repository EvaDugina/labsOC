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

int main(int argc, char** argv){

	while (1){

		struct shmid_ds shmid_struct;
	
		key_t key = ftok(MEM_NAME, 1);
		if (key < 0){
			printf("[WRITER] ftok() failed with code: %d\n", errno);
			return -1;
		}

		int shmem_id = shmget(key, sizeof(d_struct), IPC_CREAT | 0666);		

		if (shmem_id < 0) {
			printf("ERROR!! shmget() failed with code: %d\n", errno);
			return -1;
		}

		void* ptr_to_shm = shmat(shmem_id, NULL, 0);

		shmctl(shmem_id, IPC_STAT, &shmid_struct);
		
		if (shmid_struct.shm_nattch > 1){
			printf("ERROR!! [WORKER] is already exist\n");
			return -1;
		}
		
		if (ptr_to_shm == (void*)-1){
			printf("ERROR!! shmat() failed with code: %d\n\n", errno);
			return -1;
		}
			
		time_t now_time = time(NULL);
		d_struct data = {now_time, getpid()};
				
		*((d_struct*)ptr_to_shm) = data;

		sleep(4);
			
		shmdt(ptr_to_shm);
		shmctl(shmem_id, IPC_RMID, NULL);
		
		printf("----\n[WRITER]\nSended PID: %d\nSended TIME: %s\n", getpid(), ctime(&now_time));
			
	}	

	return 0;
}
