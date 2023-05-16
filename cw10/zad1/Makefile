SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all:
	make client
	make server

client: client.c
	$(CC) $(CFLAGS) client.c -o client -lpthread

server: server.c
	$(CC) $(CFLAGS) server.c -o server -lpthread

clean:
	rm -f client server