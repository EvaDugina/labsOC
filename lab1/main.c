#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void end(void){
	printf("~~~\nATEXIT!\n");
	printf("PID: %d\n", getpid());
	printf("PPID: %d\n\n", getppid());
}

int main(){
	
	atexit(end);

	pid_t pid;
	int rv = 0;
	switch(pid=fork()){
		case -1:
			perror("FORK()");
			exit(1);
		case 0:
			printf(" CHILD: Я родился!!\n");
			printf(" CHILD - MY PID: %d\n", getpid());
			printf(" CHILD - MY PPID: %d\n", getppid());
			printf(" CHILD: Прощай, Отец! Позаботься о матери, я умираю!\n");
			exit(rv);
		default:
			printf(" --- PARENT: Батя в здании!!\n");
			printf(" --- PARENT - MY PID: %d\n", getpid());
			printf(" --- PARENT - MY PPID: %d\n", getppid());
			printf(" --- PARENT: Жду, пока сынишка вызовет exit()\n...\n");
			wait(&rv);	
			printf(" --- PARENT - Код возврата потомка: %d\n", WEXITSTATUS(rv));
			printf(" --- PARENT - И я умираю следом!\n");	
	}
	
	return 0;	

}

