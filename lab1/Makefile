.PHONY: clean run

run: myLs.out
	./myLs.out -la
myLs.out: main.o lsFile.o lsList.o
	gcc main.o lsFile.o lsList.o -o myLs.out
main.o: main.c
	gcc main.c -c
lsFile.o: lsFile.c
	gcc lsFile.c -c
lsList.o: lsList.c
	gcc lsList.c -c
clean:
	rm *.o *.out
