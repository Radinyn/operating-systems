SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all:
	make integral
	make worker

integral: integral.c
	$(CC) $(CFLAGS) integral.c -o integral

worker: worker.c
	$(CC) $(CFLAGS) worker.c -o worker

report:
	make all
	echo "" > report.txt
	./integral 0.000000001 1  >> report.txt
	./integral 0.000000001 2  >> report.txt
	./integral 0.000000001 10 >> report.txt

clean:
	rm -f integral worker
