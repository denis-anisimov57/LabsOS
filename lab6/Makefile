.PHONY: clean

all: pipe.out fifo.out

pipe.out: mainPipe.c
	gcc mainPipe.c -o pipe.out -Wall -Wextra
fifo.out: mainFifo.c
	gcc mainFifo.c -o fifo.out -Wall -Wextra
clean:
	rm *.out
