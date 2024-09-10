#include "lsFile.h"

struct lsFile* lsFileInit() {
	struct lsFile* file = malloc(sizeof(struct lsFile));
	strcpy(file->permissions, "----------");
	file->user = NULL;
	file->group = NULL;
	file->name = NULL;
	file->color = WHITE;
	return file;
}

void setPermissions(struct lsFile* file, unsigned long bitMask) {
	strcpy(file->permissions, "----------");
	bool isExecutable = false;
	if((bitMask & S_IFLNK) == S_IFLNK) {
		file->permissions[0] = 'l';
		file->color = CYAN;
	}
	else if((bitMask & S_IFDIR) == S_IFDIR) {
		file->permissions[0] = 'd';
		file->color = BLUE;
	}
	if((bitMask & S_IRUSR) == S_IRUSR) {
		file->permissions[1] = 'r';
	}
	if((bitMask & S_IWUSR) == S_IWUSR) {
		file->permissions[2] = 'w';
	}
	if((bitMask & S_IXUSR) == S_IXUSR) {
		file->permissions[3] = 'x';
		isExecutable = true;
	}
	if((bitMask & S_IRGRP) == S_IRGRP) {
		file->permissions[4] = 'r';
	}
	if((bitMask & S_IWGRP) == S_IWGRP) {
		file->permissions[5] = 'w';
	}
	if((bitMask & S_IXGRP) == S_IXGRP) {
		file->permissions[6] = 'x';
		isExecutable = true;
	}
	if((bitMask & S_IROTH) == S_IROTH) {
		file->permissions[7] = 'r';
	}
	if((bitMask & S_IWOTH) == S_IWOTH) {
		file->permissions[8] = 'w';
	}
	if((bitMask & S_IXOTH) == S_IXOTH) {
		file->permissions[9] = 'x';
		isExecutable = true;
	}
	if(file->color == WHITE && isExecutable) {
		file->color = GREEN;
	}
}

void setUser(struct lsFile* file, unsigned uid) {
	struct passwd* user = getpwuid(uid);
	if(user == NULL) {
		fprintf(stderr, "User error\n");
		unsigned uidCopy = uid;
		unsigned length = 0;
		while(uidCopy != 0) {
			uidCopy /= 10;
			length++;
		}
		file->user = malloc(length + 1);
		sprintf(file->user, "%u", uid);
	}
	else {
		file->user = malloc(sizeof(user->pw_name));
		strcpy(file->user, user->pw_name);
	}
}

void setGroup(struct lsFile* file, unsigned gid) {
	struct group* gr = getgrgid(gid);
	if(gr == NULL) {
		fprintf(stderr, "Group error\n");
		unsigned gidCopy = gid;
		unsigned length = 0;
		while(gidCopy != 0) {
			gidCopy /= 10;
			length++;
		}
		file->group = malloc(length + 1);
		sprintf(file->group, "%u", gid);
	}
	else {
		file->group = malloc(sizeof(gr->gr_name));
		strcpy(file->group, gr->gr_name);
	}
}

void setMTime(struct lsFile* file, unsigned long timestamp) {
	char dayTime[6];
	struct tm ts = *localtime(&timestamp);
	strftime(dayTime, sizeof(dayTime), "%H:%M", &ts);
	char date[12];
	strftime(date, sizeof(date), "%b %e ", &ts);
	strcat(date, dayTime);
	strcpy(file->mtime, date);
}

void deleteLsFile(struct lsFile* file) {
	if(file->user != NULL) {
		free(file->user);
	}
	if(file->group != NULL) {
		free(file->group);
	}
	if(file->name != NULL) {
		free(file->name);
	}
	free(file);
}
