#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <grp.h>
#include <time.h>
#include "lsFile.h"
#include "lsList.h"

char greenColor[] = "\x1b[32m";
char resetColor[] = "\x1b[0m";

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
	int curWidth = 0;
	struct stat d_info;
	struct lsList* list = lsListInit();
	while(dir != NULL) {
		char* path = malloc(sizeof(dirname) + sizeof(dir->d_name) + sizeof("/"));
		strcpy(path, dirname);
		strcat(path, "/");
		strcat(path, dir->d_name);
		int statRes = lstat(path, &d_info);
		if(statRes != 0) {
			fprintf(stderr, "Stat error\n");
		}	
		if(dir->d_name[0] == '.' && !aFlag) {
			dir = readdir(d);
			free(path);
			continue;
		}
		if(!lFlag) {
			/*curWidth += getFilenameSize(dir->d_name);
			struct winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			if(curWidth > w.ws_col / 5) {
				printf("\n");
				curWidth = 0;
			}*/
			printf("%s ", dir->d_name);
			//printf("%s(%d %d) ", dir->d_name, w.ws_col, curWidth);
		}
		else {
			struct lsFile* file = lsFileInit();
			setPermissions(file, d_info.st_mode);
			setUser(file, d_info.st_uid);
			setGroup(file, d_info.st_gid);
			setMTime(file, d_info.st_mtime);
			file->numlink = d_info.st_nlink;
			file->size = d_info.st_size;
			file->name = malloc(sizeof(dir->d_name));
			strcpy(file->name, dir->d_name);
			addLsFile(list, file);		
		}	
		free(path);
		dir = readdir(d);
	}
	printLsList(list);
	deleteLsList(list);
	printf("\n");
	closedir(d);
	return 0;
}
