#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/wait.h>


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
			
			time_t p_time; 
			pid_t p_pid;
			
			close(pipefd[1]);
			read(pipefd[0], &p_time, sizeof(time_t));
			read(pipefd[0], &p_pid, sizeof(pid_t));
			close(pipefd[0]);

			int p_hh = localtime(&p_time)->tm_hour;
			int p_mm = localtime(&p_time)->tm_min;
			int p_ss = localtime(&p_time)->tm_sec;
		
			sleep(5);
			
			time_t ch_time;
			ch_time = time(NULL);
			int hh = localtime(&ch_time)->tm_hour;
			int mm = localtime(&ch_time)->tm_min;
			int ss = localtime(&ch_time)->tm_sec;
			
			printf("PARENT PID = %d\n", p_pid);
			printf("PARENT TIME = %s::%s::%s\n", timeToChar(p_hh), timeToChar(p_mm), timeToChar(p_ss));
			printf("CHILD TIME = %s::%s::%s\n", timeToChar(hh), timeToChar(mm), timeToChar(ss));
	
			return 0;
			
		default:
			printf("[PARENT]\n");

			time_t now_time;
			now_time = time(NULL);
			
			close(pipefd[0]);
			write(pipefd[1], &now_time, sizeof(time_t));
			write(pipefd[1], &pid, sizeof(pid_t));
			close(pipefd[1]);
						
			printf("PID и ВРЕМЯ успешно переданы ребёнку.\n");
			
			sleep(5);
			return 0;
	}
	
	return 0;
}
