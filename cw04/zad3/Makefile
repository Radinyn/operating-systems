SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all:
	make catcher
	make sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

clean:
	rm -f catcher sender
