#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include <sys/stat.h>
#include <sys/types.h>

//#define NAME_MAX 256 

void ls(char path[]){
	DIR *_DIR;
	struct dirent *_dir;

	if ((_DIR = opendir(path)) == NULL){
		printf("CANNOT OPEN PATH %s", path);
	} else {
		while ((_dir = readdir(_DIR)) != NULL){
			if (_dir->d_name[0] ==  '.'){
			} else{
				printf("%s  ", _dir->d_name);
			}
		}
	}
	closedir(_DIR);
}

void ls_l(char path[]){
	DIR *_DIR;
	struct dirent *_dir;
	struct stat _stat;
	
	struct passwd *pw; 
    	struct group *gr;
	
	char buf[1024];

	if ((_DIR = opendir(path)) == NULL){
		printf("CANNOT OPEN PATH %s", path);
	} else {
		if(path[strlen(path)-1] == '/'){
			struct stat _stat_dir;
			char *new_path = (char *) calloc(strlen(path),sizeof(char));
			strncpy(new_path, path, strlen(path)-1);
			stat(new_path, &_stat_dir);
			printf("Total: %ld\n", _stat_dir.st_blocks);
		}
		else{
			struct stat _stat_dir;
			stat(path, &_stat_dir);
			printf("Total: %ld\n", _stat_dir.st_blocks);
		}
		while ((_dir = readdir(_DIR)) != NULL){
			if (_dir->d_name[0] == '.'){
			} else{
				sprintf(buf, "%s%s", path, _dir->d_name);		
				//printf("%s\n", buf);
				stat(buf, &_stat);
				switch (_stat.st_mode & S_IFMT){
            				case S_IFBLK: printf("b"); break;
            				case S_IFCHR: printf("c"); break; 
            				case S_IFDIR: printf("d"); break;
            				case S_IFIFO: printf("p"); break; 
            				case S_IFLNK: printf("l"); break; 
            				case S_IFSOCK: printf("s"); break;
            				default: printf("-"); break;
                		}

        			printf((_stat.st_mode & S_IRUSR) ? " r" : " -");
        			printf((_stat.st_mode & S_IWUSR) ? "w" : "-");
        			printf((_stat.st_mode & S_IXUSR) ? "x" : "-");
        			printf((_stat.st_mode & S_IRGRP) ? "r" : "-");
        			printf((_stat.st_mode & S_IWGRP) ? "w" : "-");
        			printf((_stat.st_mode & S_IXGRP) ? "x" : "-");
        			printf((_stat.st_mode & S_IROTH) ? "r" : "-");
        			printf((_stat.st_mode & S_IWOTH) ? "w" : "-");
	   	     		printf((_stat.st_mode & S_IXOTH) ? "x" : "-");

				printf("  %d", _stat.st_nlink);

				pw = getpwuid(_stat.st_uid);
				if(pw != 0 && pw->pw_uid && pw->pw_gecos && pw->pw_name){
					printf("  %s", pw->pw_name);
				}	

				gr = getgrgid(_stat.st_gid);
        			printf("  %s", gr->gr_name);	
				
				printf("  %ld",_stat.st_size);
				
				time_t year;
				time(&year);
				switch (localtime(&_stat.st_ctime)->tm_mon){
					case 0: printf("  Jan"); break;
					case 1: printf("  Feb"); break;
					case 2: printf("  Mar"); break;
					case 3: printf("  Apr"); break;
					case 4: printf("  May"); break;
					case 5: printf("  Jun"); break;
					case 6: printf("  Jul"); break;
					case 7: printf("  Aug"); break;
					case 8: printf("  Sep"); break;
					case 9: printf("  Oct"); break;
					case 10:printf("  Nov"); break;
					case 11:printf("  Dec"); break;
				}
				
				printf("  %d", localtime(&_stat.st_ctime)->tm_mday);
				
				//printf("  %d == %d", localtime(&year)->tm_year, localtime(&_stat.st_ctime)->tm_year);
				if(localtime(&year)->tm_year != localtime(&_stat.st_ctime)->tm_year)
        				printf("  %d", localtime(&_stat.st_ctime)->tm_year + 1900);
				else {
					printf("  %d", localtime(&_stat.st_ctime)->tm_hour);
					printf(":%d", localtime(&_stat.st_ctime)->tm_min);
				}

				printf("  %s", _dir->d_name);
				printf("\n");
			}
		}
	}
	closedir(_DIR);
}


int main(int argc, char **argv){
	
	DIR *_DIR;
	struct dirent *_dir;
	char *path;

	if(argc == 1){
		//printf("%s", "ls\n");
		path = "./";
		ls(path);
		printf("\n");
	}
		
	else if(argc == 2){
		//printf("%s", "ls+path or ls+l");
		int opt;
		int gopt = getopt(argc, argv, "l");
		if(gopt != -1){
			//printf("%s", "ls+l");
			path = "./";
			ls_l(path);
			
		} else {
			//printf("%s", "ls+path");
			ls(argv[1]);
			printf("\n");
		}	
	}
	
	else if(argc == 3){
		if(strlen(argv[1]) == 2 && argv[1][0] == '-' && argv[1][1] == 'l'){
			//printf("%s", "ls+l+path");
			ls_l(argv[2]);

		}else if(strlen(argv[2]) == 2 && argv[2][0] == '-' && argv[2][1] == 'l'){
			//printf("%s", "ls+path+l");
			ls_l(argv[1]);
			
		}else{
			printf("%s", "nothing");
		} 
	}
	
	else{
		//printf("%s", "nothing");
	}	
	
	//printf("\n");
	return 0;
}	
