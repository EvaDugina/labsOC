#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>


char *f_path;
int count_flags;

char flag1;
char flag3_1, flag3_2, flag3_3;


// Вспомогательные функции
int isFlagsCorrect() {
	
	if (flag1 != 'u' && flag1 != 'g' && flag1 != 'o')
		return -1; 

	switch(count_flags){
		case 4:
			if (flag3_1 == 'r' || flag3_1 == 'w' || flag3_1 == 'x')
				return 1; 
			break;

		case 5:		
			if ((flag3_1 == 'r' || flag3_1 == 'w' || flag3_1 == 'x') && (flag3_2 == 'r' || flag3_2 == 'w' || flag3_2 == 'x') && flag3_1 != flag3_2)
				return 1; 
			break;

		case 6:
			if ((flag3_1 == 'r' || flag3_1 == 'w' || flag3_1 == 'x') && (flag3_2 == 'r' || flag3_2 == 'w' || flag3_2 == 'x') && 
				flag3_1 != flag3_2 && flag3_2 != flag3_3 && flag3_1 != flag3_3)
				return 1; 
			break;

		default:
			return -1;
	}
	return -1;
}


int add_by_flag(char flag1, char flag2) {

	struct stat stat_file;
	stat(f_path, &stat_file);
	
	mode_t buf_permissions = stat_file.st_mode;
	
	int output;
	
	switch (flag1){
		case 'u':
			if (flag2 == 'r') {
				output = chmod(f_path, S_IRUSR | buf_permissions);
				//printf("Добавлено для USER право на READ\n");
			}
			else if (flag2 == 'w') {
				output = chmod(f_path, S_IWUSR | buf_permissions);
				//printf("Добавлено для USER право на WRITE\n");
			}
			else {
				output = chmod(f_path, S_IXUSR | buf_permissions);
				//printf("Добавлено для USER право на EXECUTE\n");
			}
			break;
		
		case 'g':				
			if (flag2 == 'r') {
				output = chmod(f_path, S_IRGRP | buf_permissions);
				//printf("Добавлено для GROUP право на READ\n");
			}
			else if (flag2 == 'w') {
				output = chmod(f_path, S_IWGRP | buf_permissions);
				//printf("Добавлено для GROUP право на WRITE\n");
			}
			else {
				output = chmod(f_path, S_IXGRP | buf_permissions);
				//printf("Добавлено для GROUP право на EXECUTE\n");
				
			}
			break;
			
		case 'o':
			if (flag2 == 'r') {
				output = chmod(f_path, S_IROTH | buf_permissions);
				//printf("Добавлено для O право на R\n");
			}
			else if (flag2 == 'w') {
				output = chmod(f_path, S_IWOTH | buf_permissions);
				//printf("Добавлено для O право на W\n");
			}
			else{
				output = chmod(f_path, S_IXOTH | buf_permissions);	
				//printf("Добавлено для O право на X\n");
			}
			break;
			
		deafult:
			//printf("ERROR! Некорректный ввод флагов!\n");
			return -1;
			break;
	}
	return output;
}

int remove_by_flag(char flag1, char flag2) {
	
	struct stat stat_file;
	stat(f_path, &stat_file);
	
	mode_t buf_permissions = stat_file.st_mode;
		
	int output;
	switch (flag1){
		case 'u':
			if (flag2 == 'r')
				output = chmod(f_path, (~S_IRUSR) & buf_permissions);
			else if (flag2 == 'w')
				output = chmod(f_path, (~S_IWUSR) & buf_permissions);
			else
				output = chmod(f_path, (~S_IXUSR) & buf_permissions);
			break;
		
		case 'g':				
			if (flag2 == 'r')
				output = chmod(f_path, (~S_IRGRP) & buf_permissions);
			else if (flag2 == 'w')
				output = chmod(f_path, (~S_IWGRP) & buf_permissions);
			else
				output = chmod(f_path, (~S_IXGRP) & buf_permissions);
			break;
			
		case 'o':
			if (flag2 == 'r')
				output = chmod(f_path, (~S_IROTH) & buf_permissions);
			else if (flag2 == 'w')
				output = chmod(f_path,(~S_IWOTH) & buf_permissions);
			else
				output = chmod(f_path, (~S_IXOTH) & buf_permissions);	
			break;
			
		deafult:
			printf("ERROR! Некорректный ввод флагов!\n");
			return -1;
			break;
	}
	//printf("OUTPUT = %d\n", output);
	return output;
}


// Основные функции работы с CHMOD
void _add() {
	//printf("_ADD:\nCOUNT_FLAGS = %d\n", count_flags);
	
	if (isFlagsCorrect() == -1) {
		printf("ERROR! Некорректный ввод флагов!\n");
		exit(1);
	}
	if (access(f_path, 0) != 0){
		printf("ERROR! Файла с именем '%s' не существует!\n", f_path);
		exit(1);
	} 

	int result = 1;
	if (count_flags == 4){	
		result *= add_by_flag(flag1, flag3_1);	
	} else if (count_flags == 5) {
		result *= add_by_flag(flag1, flag3_1);
		result *= add_by_flag(flag1, flag3_2);
		
	} else {
		result *= add_by_flag(flag1, flag3_1);
		result *= add_by_flag(flag1, flag3_2);
		result *= add_by_flag(flag1, flag3_3);
	}
	
	if (result != 0)
		printf("Ошибка прав доступа для файла '%s'!\n", f_path);
	else 
		printf("Права доступа для файла '%s' успешно изменены!\n", f_path);
	
}

void _remove() {
	//printf("_REMOVE:\nCOUNT_FLAGS = %d\n", count_flags);

	if (isFlagsCorrect() == -1){
		printf("ERROR! Некорректный ввод флагов!\n");
		exit(1);
	}
	if (access(f_path, 0) != 0){
		printf("ERROR! Файла с именем '%s' не существует!\n", f_path);
		exit(1);
	}

	int result = 1;
	if (count_flags == 4){	
		result *= remove_by_flag(flag1, flag3_1);	
	} else if (count_flags == 5) {
		result *= remove_by_flag(flag1, flag3_1);
		result *= remove_by_flag(flag1, flag3_2);
	} else {
		result *= remove_by_flag(flag1, flag3_1);
		result *= remove_by_flag(flag1, flag3_2);
		result *= remove_by_flag(flag1, flag3_3);
	}

	if (result != 0)
		printf("Ошибка прав доступа для файла '%s'!\n", f_path);
	else 
		printf("Права доступа для файла '%s' успешно изменены!\n", f_path);
	
}

void _help(){
	printf("РУКОВОДСТВО ПО ИСПОЛЬЗОВАНИЮ ПРОГРАММЫ:\n");
	printf("./chmod -первый_флаг&второй_флаг&третий_флаг название_файла\n");
	printf("ФЛАГИ ПИШУТСЯ СЛИТНО, БЕЗ РАЗДЕЛИТЕЛЕЙ!\n");
	printf("первый_флаг = u[user] tor g[group] or o[other];\n");
	printf("второй_флаг = a[add] or r[remove]\n");
	printf("третий_флаг = r[read] and|or w[write] and|or x[execute]\n");
}


int main(int argc, char **argv){
	
	if (argc == 3 && argv[1][0] == '-'){
		count_flags = strlen(argv[1]);
		if (count_flags == 4){

			flag1 = argv[1][1];
			flag3_1 = argv[1][3];
			f_path = argv[2];
			
			if (argv[1][2] == 'a')
				_add();
			else if (argv[1][2] == 'r')
				_remove();
		
		} else if (count_flags == 5){
			
			flag1 = argv[1][1];
			flag3_1 = argv[1][3];
			flag3_2 = argv[1][4];
			f_path = argv[2];		
						
			if (argv[1][2] == 'a')
				_add();
			else if (argv[1][2] == 'r')
				_remove();
			
		} else if (count_flags == 6){
			
			flag1 = argv[1][1];
			flag3_1 = argv[1][3];
			flag3_2 = argv[1][4];
			flag3_3 = argv[1][5];
			f_path = argv[2];
			
			if (argv[1][2] == 'a')
				_add();
			else if (argv[1][2] == 'r')
				_remove();
			
		} else{
			printf("ERROR! Некорректный ввод команды!\n");
		}
		
	} else if(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h' && strlen(argv[1]) == 2){
		_help();
	} else {
		printf("ERROR! Некорректный ввод команды!\n");
	}	

	return 0;
}
