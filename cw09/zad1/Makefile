SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all: santa.c
	$(CC) $(CFLAGS) santa.c -o santa -lpthread 

clean:
	rm -f santa
