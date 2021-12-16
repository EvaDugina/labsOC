#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>


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
		printf("ОШИБКА! Некорректный ввод данных!");
		return -1;
	}

	int pipefd[2];
	if (pipe(pipefd) == -1){
		printf("ОШИБКА!! pipe()\n");
		return -1;
	}

	pid_t pid;
	switch(pid = fork()){
		case -1:
			printf("ОШИБКА!! fork()\n");
			return -1;
		case 0:
			printf("[CHILD]\n");
			
			sleep(5);
			
			d_struct g_data;
			
			close(pipefd[1]);
			read(pipefd[0], &g_data, sizeof(d_struct));
			close(pipefd[0]);

			int p_hh = localtime(&g_data.time)->tm_hour;
			int p_mm = localtime(&g_data.time)->tm_min;
			int p_ss = localtime(&g_data.time)->tm_sec;
			
			time_t ch_time;
			ch_time = time(NULL);
			int hh = localtime(&ch_time)->tm_hour;
			int mm = localtime(&ch_time)->tm_min;
			int ss = localtime(&ch_time)->tm_sec;
			
			printf("PARENT PID = %d\n", g_data.pid);
			printf("PARENT TIME = %s::%s::%s\n", timeToChar(p_hh), timeToChar(p_mm), timeToChar(p_ss));
			printf("CHILD TIME = %s::%s::%s\n", timeToChar(hh), timeToChar(mm), timeToChar(ss));
	
			return 0;
			
		default:
			printf("[PARENT]\n");
			
			time_t now_time;
			now_time = time(NULL);
			d_struct data = {now_time, pid};
			
			close(pipefd[0]);
			write(pipefd[1], &data, sizeof(d_struct));
			close(pipefd[1]);
						
			printf("PID и ВРЕМЯ успешно переданы ребёнку.\n");
			
			sleep(5);
			return 0;
	}
	
	return 0;
}
