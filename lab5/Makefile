.PHONY: clean

archiver.out: main.o archiver.o
	gcc main.o archiver.o -o archiver.out
main.o: main.c
	gcc main.c -c -Wall -Wextra
archiver.o: archiver.c
	gcc archiver.c -c -Wall -Wextra
clean:
	rm *.o *.out
