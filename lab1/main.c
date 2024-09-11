#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "lsList.h"

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
				fprintf(stderr, "Unknown op\n");
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
	struct stat d_info;
	struct lsList* list = lsListInit();
	list->lFlag = lFlag;
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
		struct lsFile* file = lsFileInit();
		setPermissions(file, d_info.st_mode);
		file->name = malloc(sizeof(dir->d_name));
		strcpy(file->name, dir->d_name);
		if(lFlag) {
			setUser(file, d_info.st_uid);
			setGroup(file, d_info.st_gid);
			setMTime(file, d_info.st_mtime);
			file->numlink = d_info.st_nlink;
			file->size = d_info.st_size;
			if(file->permissions[0] == 'l') {
					int linkRes = readlink(path, file->linkTo, sizeof(file->linkTo));
					if(linkRes == -1) {
						fprintf(stderr, "Link error\n");
					}
					file->linkTo[linkRes] = '\0';
			}
		}
		addLsFile(list, file);	
		free(path);
		dir = readdir(d);
	}
	printLsList(list);
	deleteLsList(list);
	closedir(d);
	return 0;
}
