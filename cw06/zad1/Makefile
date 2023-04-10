SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all:
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f server client