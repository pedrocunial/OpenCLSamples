CC = gcc
CFLAGS = -Wall
CLIBS = -lOpenCL

all: basic.o
	$(CC) $(CFLAGS) -o sample basic.o $(CLIBS)

hello.o: basic.c rot13.cl
	$(CC) $(CFLAGS) -c basic.c

.PHONY: clean

clean:
	rm *.o sample
