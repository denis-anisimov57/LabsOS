#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

char* convertDate(unsigned long* timestamp) {
	char* dayTime = malloc(6);
	struct tm ts = *localtime(timestamp);
	strftime(dayTime, sizeof(dayTime), "%H:%M", &ts);
	char* date = malloc(15);
	strftime(date, sizeof(date), "%b %e ", &ts);
	strcat(date, dayTime);
	free(dayTime);
	return date;
}

int main(int argc, char** argv) {
	char c;
	bool lFlag = false, aFlag = false;
      	while((c = getopt(argc, argv, "al")) != -1) {
		switch(c) {
			case 'a': {
				aFlag = true;
				break;
			}
			case 'l':
				lFlag = true;
				break;
			default:
				printf("Unknown op\n");
				break;
		}
	
	}
	char* dirname;
	if(optind >= argc) {
		dirname = ".";
	}
	else {
		dirname = argv[optind];
	}
	DIR* d = opendir(dirname);
	if(d == NULL) {
		fprintf(stderr, "Can't open dir\n");
		return -1;
	}
	struct dirent* dir = readdir(d);
	int count = 1;
	struct stat d_info;
	while(dir != NULL) {
		char* path = malloc(sizeof(dirname) + sizeof(dir->d_name) + sizeof("/"));
		strcpy(path, dirname);
		strcat(path, "/");
		strcat(path, dir->d_name);
		int statRes = stat(path, &d_info);
		if(statRes != 0) {
			fprintf(stderr, "Stat error\n");
		}
		if(dir->d_name[0] == '.' && !aFlag) {
			dir = readdir(d);
			continue;
		}
		if(!lFlag) {
			printf("%s  ", dir->d_name);
		}
		else {
			printf("(permissions) %lu ", d_info.st_nlink);
			struct passwd* user = getpwuid(d_info.st_uid);
			if(user == NULL) {
				fprintf(stderr, "User error\n");
				printf("%u ", d_info.st_uid);
			}
			else {
				printf("%s ", user->pw_name);
			}
			struct group* gr = getgrgid(d_info.st_gid);
			if(gr == NULL) {
				fprintf(stderr, "Group error\n");
				printf("%u ", d_info.st_gid);
			}
			else {
				printf("%s ", gr->gr_name);
			}
			char* buf = convertDate(&d_info.st_mtime);
			printf("%lu %s %s\n", d_info.st_size, buf, dir->d_name);
			free(buf);
		}

		//printf("mode: %u %u\n", d_info.st_mode, d_info.st_mode & S_IXUSR);
		dir = readdir(d);
		free(path);
	}
	printf("\n");
	closedir(d);

	return 0;
}
