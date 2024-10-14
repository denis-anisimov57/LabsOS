#include "archiver.h"
#include <stdio.h>
#include <string.h>

void errMsg() {
	fprintf(stderr, "Incorrect argument\n");
	printf("Try ./archiver.out -h or ./archiver.out --help\n");		
}

void printManual() {
	printf("./archiver.out -h(--help)				print manual to archiver.out\n"
	"./archiver.out <archiveName> -s(--stat)			print archive status\n"
	"./archiver.out <archiveName> -i(--input) <file>		add <file> to <archiveName>\n"
	"./archiver.out <archiveName> -e(--extract) <file>	extract <file> from <archiveName> to current directory\n");
}

int main(int argc, char** argv) {
	if(argc < 2) {
		errMsg();
		return -1;
	}
	if(argc == 2) {
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			printf("Manual:\n");
			printManual();
			return 0;
		}
		else {
			errMsg();
			return -1;
		}
	}
	struct Archiver* ar = initArchiver(argv[1]);
	if(argc == 3) {
		if(strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "--stat") == 0) {
			printf("Archive status:\n");
			printArchiver(ar);
		}
		else {
			errMsg();
		}
	}
	else if(argc == 4) {
		if(strcmp(argv[2], "-i") == 0 || strcmp(argv[2], "--input") == 0) {
			addToArchiver(ar, argv[3]);
			saveArchive(ar);
		}
		else if(strcmp(argv[2], "-e") == 0 || strcmp(argv[2], "--extract") == 0) {
			removeFromArchiver(ar, argv[3], true);
			saveArchive(ar);
		}
		else {
			errMsg();
		}
	}
	else {
		errMsg();
	}
	delArchiver(ar);
	return 0;
}	
