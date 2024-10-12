#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int parseModeMask(const char* str, const char* path) {
	struct stat st;
	int statRes = stat(path, &st);
	if(statRes == -1) {
		fprintf(stderr, "Stat error\n");
		return -1;
	}
	mode_t prevMode = st.st_mode;
	unsigned ugo = 0, userOpPerm = 0, op = 0, perm = 0;
	for(size_t i = 0; i < strlen(str); i++) {
		if(userOpPerm == 0) {
			switch(str[i]) {
				case 'u': {
					ugo = ugo | 0b100;
					break;
				}
				case 'g': {
					ugo = ugo | 0b010;
					break;
				}
				case 'o': {
					ugo = ugo | 0b001;
					break;
				}
				case 'a': {
					ugo = 0b111;
					break;
				}
				default: {
					if(ugo == 0) {
						ugo = 0b111;
					}
					userOpPerm = 1;
					break;
				}
			}
		}
		if(userOpPerm == 1) {
			switch(str[i]) {
				case '+': {
					op = 1;
					break;
				}
				case '-': {
					op = 2;
					break;
				}
				case '=': {
					op = 3;
					break;
				}
				default: {
					userOpPerm = 2;
					break;
				}
			}
		}
		if(userOpPerm == 2) {
			switch(str[i]) {
				case 'r': {
					perm = perm | 0b100;
					break;
				}
				case 'w': {
					perm = perm | 0b010;
					break;
				}
				case 'x': {
					perm = perm | 0b001;
					break;
				}
				default: {
					fprintf(stderr, "Incorrect mode\n");
					return -1;
					break;
				}
			}
		}	
	}
	if(op == 0) {
		fprintf(stderr, "Incorrect mode\n");
		return -1;
	}
	int mode = 0;
	if((ugo & 0b100) == 0b100) {
		mode = mode | (perm << 6);
	}
	if((ugo & 0b010) == 0b010) {
		mode = mode | (perm << 3);
	}
	if((ugo & 0b001) == 0b001) {
		mode = mode | perm;
	}
	if(op == 1) {
		mode = mode | prevMode;
	}
	else if(op == 2) {
		mode = prevMode & (~mode); 
	}
	return mode;
}

int parseMode(const char* str, const char* path) {
	for(size_t i = 0; i < strlen(str); i++) {
		if(str[i] > '7' || str[i] < '0') {
			int mode = parseModeMask(str, path);
			return mode;
		}
	}
	if(strlen(str) > 3) {
		if(atoi(str) == 0) {
			return 0;
		}
		else {
			fprintf(stderr, "Incorrect mode\n");
			return -1;
		}
	}
	int mode = 0;
	int digit = str[0] - 48;
	digit = digit << 6;
	mode = mode | digit;
	digit = str[1] - 48;
	digit = digit << 3;
	mode = mode | digit;
	digit = str[2] - 48;
	mode = mode | digit;
	return mode;
}

int main(int argc, char** argv) {
	if(argc < 3) {
		fprintf(stderr, "Not enough arguments\n");
		return -1;
	}
	int mode = parseMode(argv[1], argv[2]);
	int res = 0;
	if(mode != -1) {	
		res = chmod(argv[2], mode);
	}
	if(res == -1) {
		fprintf(stderr, "fchmod error\n");
	}
	return 0;
}
