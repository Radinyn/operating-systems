SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all: sigtest.c
	$(CC) $(CFLAGS) sigtest.c -o sigtest

report:
	echo "Report is just a copied output, signals can't be used well with make."

clean:
	rm -f sigtest
