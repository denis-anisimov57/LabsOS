#pragma once
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

struct lsFile {
	char permissions[10];
	unsigned long numlink;
	char* user;
	char* group;
	unsigned long size;
	char mtime[12];
	char* name;
};

struct lsFile* lsFileInit();

void setPermissions(struct lsFile* file, unsigned long bitMask);

void setUser(struct lsFile*file, unsigned uid);

void setGroup(struct lsFile* file, unsigned gid);

void setMTime(struct lsFile* file, unsigned long timestamp);

void deleteLsFile(struct lsFile* file);
