SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all: integral.c
	$(CC) $(CFLAGS) integral.c -o integral

report:
	make all
	echo "" > report.txt
	./integral 0.000000001 1  >> report.txt
	./integral 0.000000001 2  >> report.txt
	./integral 0.000000001 10 >> report.txt

clean:
	rm -f integral
