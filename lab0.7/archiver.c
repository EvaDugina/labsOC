#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/stat.h>


char *filename;
char *arch_name;


// Реализация дополнительных функций
long getFSize(char *f_name){
	//printf("+ getFSize()\n");
	FILE *file = fopen(f_name, "rb");
	if (!file)
		return -1;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fclose(file);
	//printf("- getFSize()\n");
	return size;
}

int countDigits(double n){
	int count = 0;
	while (n >= 10){
		count++;
		n /= 10;
	}	
	return count+1;
}

int isFExists(){
	//printf("isFExists()\n");
	FILE* f_arch;
	if(!(f_arch = fopen(arch_name, "r"))){
		//printf("ФАЙЛ АРХИВА ОТСУТСВУЕТ!\n");
		return 0;
	}		

	int buf_name_size = 0;
	int buf_data_size = 0;
	char *buf_name;
	
	int flag_now_chapter = 0;
	
	while ((fscanf(f_arch, "%d", &buf_name_size)) != EOF){
		
		buf_name = (char *) malloc(buf_name_size);
		
		fseek(f_arch, 1, SEEK_CUR);
		
		fread(buf_name, 1, buf_name_size, f_arch);
		if (strcmp(buf_name, filename) == 0)
			return 1;
		//printf("%s ?? %s = %d (%d, %d)\n", buf_name, filename, strcmp(buf_name, filename), strlen(buf_name), strlen(filename));
		
		fseek(f_arch, 1, SEEK_CUR);

		char *st_mode;
		fscanf(f_arch, "%d", &st_mode);

		fseek(f_arch, 1, SEEK_CUR);
		
		fscanf(f_arch, "%d", &buf_data_size);	
		
		fseek(f_arch, 1+buf_data_size, SEEK_CUR);	
			
	}
	fclose(f_arch);
	
	return 0;
}

int isCommand(char* argv, char* command){
	int size = strlen(argv);
	if (size == 2 && argv[1] == command[0] && argv[0] == '-')
		return 1;

	if (size-2 != strlen(command))
		return 0;

	for(int i = 2; i < size; i++){
		if (argv[i] != command[i-2])
			return 0;
	}
	return 1;
}


// Реализация основных функций
void _input(){
	//printf("~INPUT\nARCH_NAME: %s\nFILE_NAME: %s\n", arch_name, filename);
	
	FILE* f_input = fopen(filename, "rb");
	if(!f_input){
		printf("ERROR: Файл '%s' не найден!\n", filename);
		exit(1);
	} else if (isFExists() == 1){
		printf("ERROR: Файл '%s' уже заархивирован!\n", filename);
		exit(2);
	}
	
	long size = getFSize(filename);
	
	int bytes_size = 0;
	
	//printf("Размер архивируемого файла: %d\n", size);
	
	struct stat file_stat;
	stat(filename, &file_stat);
		
	char output_file_size[countDigits(size)+1];
	sprintf(output_file_size, "%d", size);
	
	char char_filename_size [countDigits(strlen(filename))+1];
	sprintf(char_filename_size, "%d", strlen(filename));	
	
	FILE* f_arch = fopen(arch_name, "a+");
	
	bytes_size += countDigits(strlen(filename));
	bytes_size += countDigits(size);
	bytes_size += strlen(filename);
	
	//char* str_info = (char*) malloc(6 + countDigits(bytes_size) + 1);
	fprintf(f_arch, "%s ", char_filename_size);
	fprintf(f_arch, "%s ", filename);
	fprintf(f_arch, "%d ", file_stat.st_mode);
	fprintf(f_arch, "%s ", output_file_size);
	
	//fputs(str_info, f_arch);

	//printf("ИНФОРМАЦИЯ О ФАЙЛЕ: %s\n", str_info);
	
	if (size != 0) {
		char byte[1];
		while(feof(f_input) == 0){
			if(fread(byte, 1, 1, f_input) == 1)
				fprintf(f_arch, "%c", byte[0]);
		}
	}

	fclose(f_input);
	fclose(f_arch);

	printf("%s успешно заархивирован!\n", filename);
	
}

void _extract(){
	//printf("~EXTRACT\nARCH_NAME: %s\nFILE_NAME: %s\n", arch_name, filename);
	FILE* f_arch;
	
	if (!(f_arch = fopen(arch_name, "rb"))){
		printf("ERROR: Неверное название архива!\n");
		exit(1);
	}		

	FILE *f_new_file;

	long end_1part = -1;
	long start_2part = -1;
	
	int buf_name_size = 0;
	int buf_data_size = 0;
	char *buf_name;
	char *buf_data;
	
	__mode_t st_mode;

	while ((fscanf(f_arch, "%d", &buf_name_size)) != EOF){
		 
		buf_name = (char *) malloc(buf_name_size + 1);
		
		fseek(f_arch, 1, SEEK_CUR);

		fread(buf_name, 1, buf_name_size, f_arch);
		if (strcmp(buf_name, filename) == 0) {
			end_1part = ftell(f_arch) - strlen(filename) - 1 - countDigits(buf_name_size);
			f_new_file = fopen(filename, "w");
		}
		//printf("%s ?? %s = %d (%d, %d)\n", buf_name, filename, strcmp(buf_name, filename), strlen(buf_name), strlen(filename));
		
		fseek(f_arch, 1, SEEK_CUR);

		fscanf(f_arch, "%d", &st_mode);

		fseek(f_arch, 1, SEEK_CUR);

		fscanf(f_arch, "%d", &buf_data_size);	
		printf("%d\n", buf_data_size);
		
		if (end_1part != -1){
			chmod(filename, st_mode);
			fseek(f_arch, 1, SEEK_CUR);

			buf_data = (char *) malloc(buf_data_size);
			fread(buf_data, 1, buf_data_size, f_arch);
			printf("BUF_DATA:\n%s\n", buf_data);			
			
			start_2part = ftell(f_arch);
			//if(buf_data_size == 0)
				//start_2part--;

			break;

		} else {
			fseek(f_arch, 1+buf_data_size, SEEK_CUR);
		}
			
	}
	//fclose(f_arch);
	
	printf("END_1PART: %d\nSTART_2PART: %d\n", end_1part, start_2part);

	long size = getFSize(arch_name);
	printf("SIZE: %d\n", size);

	fseek(f_arch, 0, SEEK_SET);

	// Вырезаемый файл находится в начале
	if(end_1part == 0 && start_2part != -1){
		printf("Вырезать файл из начала:\n");	
			
		char *s_part = (char *) malloc(sizeof(char *) * (size - start_2part));
		
		fseek(f_arch, start_2part, SEEK_CUR);

		//if (buf_data_size == 0)
			//fseek(f_arch, 2, SEEK_CUR);

		fread(s_part, 1, size-start_2part, f_arch);
		fclose(f_arch);
		
		
		f_arch = fopen(arch_name, "w");
		fwrite(s_part, 1, strlen(s_part), f_arch);
		printf("----------\nF_ARCHIVE:\n%s\n", s_part);

		fwrite(buf_data, 1, buf_data_size, f_new_file);
		printf("----------\nIN NEW_FILE:\n%s", buf_data);
		fclose(f_new_file);

		
	}

	// Вырезаемый файл находится в конце
	else if (end_1part != -1 && start_2part == size){
		printf("Вырезать файл из конца:\n");
		
		char *f_part = (char *) malloc(sizeof(char *) * (end_1part));
		
		fread(f_part, 1, end_1part, f_arch);
		fclose(f_arch);
			
		f_arch = fopen(arch_name, "w");
		fwrite(f_part, 1, strlen(f_part), f_arch);
		printf("----------\nF_ARCHIVE:\n%s", f_part);

		fwrite(buf_data, 1, buf_data_size, f_new_file);
		printf("----------\nIN NEW_FILE:\n%s\n", buf_data);
		fclose(f_new_file);
		
	}

	// Вырезаемый файл находтся в середине
	else if (end_1part != -1 && start_2part != -1){	
		printf("Вырезать файл из середины:\n");
		
		char *f_part = (char *) malloc(sizeof(char *) * (end_1part));
		char *s_part = (char *) malloc(sizeof(char *) * (size - start_2part));

		fread(f_part, 1, end_1part, f_arch);
		fseek(f_arch, start_2part, SEEK_SET);
		
		//if (buf_data_size == 0)
			//fseek(f_arch, 2, SEEK_CUR);

		fread(s_part, 1, size-start_2part, f_arch);
		fclose(f_arch);
			
		f_arch = fopen(arch_name, "w");
		fprintf(f_arch, "%s", f_part);
		fprintf(f_arch, "%s", s_part);
		printf("----------\nF_ARCHIVE:\n%s\n++++++++++++++\n%s\n", f_part, s_part);

		fprintf(f_new_file, "%s", buf_data);
		printf("----------\nIN NEW_FILE:\n%s", buf_data);
		fclose(f_new_file);
		
	} 

	else {
		printf("Файл '%s' отсутствует в архиве!\n", filename);
	}

	fclose(f_arch);
}

void _stat(){


	
}

void _help(){
	printf("СПИСОК ДОСТУПНЫХ КОМАНД:\n");
	printf("./archiver <имя архива> -i(--input) <имя файла>\t// Заархивировать файл\n");
	printf("./archiver <имя архива> -e(--extract) <имя файла>\t// Разархивировать файл\n");
	printf("./archiver -s(--stat)\t// Вывести текущее состояние архива\n");
	printf("./archiver -h(--help)\t// Вывести справку по работе с архиватором\n");
}



int main(int argc, char** argv){

	//printf("123456789 - %d\n", countDigits(123456789));

	if(argc == 2){
		if (isCommand(argv[1], "help") == 1){
			_help();
		} else if (isCommand(argv[1], "stat") == 1){
			_stat();	 
		} else{	
			printf("ERROR: Несуществующие команды!\n\n");
			_help();
		}
	} else if(argc == 4){
		if (isCommand(argv[2], "input") == 1){
			arch_name = argv[1];
			filename = argv[3];
			_input();
		} else if (isCommand(argv[2], "extract") == 1){
			arch_name = argv[1];
			filename = argv[3];
			_extract();
		} else {
			printf("ERROR: Несуществующие команды!\n\n");
			_help();
		}	
	} else {
		printf("ERROR: Несуществующие команды!\n\n");
		_help();
	}

	return 0;
}
