.PHONY: clean

all: pipe.out fifoFork.out fifo.out

fifo.out: mainFifo.c
	gcc mainFifo.c -o fifo.out -Wall -Wextra
pipe.out: mainPipe.c
	gcc mainPipe.c -o pipe.out -Wall -Wextra
fifoFork.out: mainFifoFork.c
	gcc mainFifoFork.c -o fifoFork.out -Wall -Wextra
clean:
	rm *.out
