SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all:
	$(CC) $(CFLAGS) server.c -o server -lrt
	$(CC) $(CFLAGS) client.c -o client -lrt

clean:
	rm -f server client