#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FIFO_NAME "fifo"


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

	if (argc != 1) {
		printf("ERROR! Incorrect data entry!");
		return -1;
	}

	pid_t pid;
	switch(pid = fork()){
		
		case -1:
			printf("ERROR!! fork()\n");
			return -1;
		
		case 0:
			sleep(1);
			printf("[CHILD]\n");
			
			int r_fifo = open(FIFO_NAME, O_RDONLY);
			if (r_fifo < 0){
				printf("ERROR!! read & open()\n");
				return -1;
			}

			sleep(5);

			d_struct g_data;
			read(r_fifo, &g_data, sizeof(d_struct));
			close(r_fifo);

			int p_hh = localtime(&g_data.time)->tm_hour;
			int p_mm = localtime(&g_data.time)->tm_min;
			int p_ss = localtime(&g_data.time)->tm_sec;
			
			time_t ch_time = time(NULL);
			int hh = localtime(&ch_time)->tm_hour;
			int mm = localtime(&ch_time)->tm_min;
			int ss = localtime(&ch_time)->tm_sec;
			
			printf("PARENT PID = %d\n", g_data.pid);
			printf("PARENT TIME = %s::%s::%s\n", timeToChar(p_hh), timeToChar(p_mm), timeToChar(p_ss));
			printf("CHILD TIME = %s::%s::%s\n", timeToChar(hh), timeToChar(mm), timeToChar(ss));
	
			
			
			return 0;
		
		default:
			printf("[PARENT]\n");

			unlink(FIFO_NAME);
			int fifo_fd = mkfifo(FIFO_NAME, 0777);
			printf("%d\n", fifo_fd);
			if (fifo_fd < 0) {
				printf("ERROR!! mkfifo()\n");
				return -1;
			}
			
			int w_fifo = open(FIFO_NAME, O_WRONLY);
			if (w_fifo < 0){
				printf("ERROR!! write & open()\n");
				return -1;
			}
			
			time_t now_time = time(NULL);
			d_struct data = {now_time, pid};
			
			write(w_fifo, &data, sizeof(d_struct));
			close(w_fifo);
			
			wait(0);			

			return 0;
	}	

	return 0;
}
