CC=gcc
CFLAGS=-Wall -O3

all: 
	make std
	make sys

std: replace.c
	$(CC) $(CFLAGS) replace.c -o replace_std

sys: replace.c
	$(CC) $(CFLAGS) replace.c -o replace_sys -D SYS

clean:
	rm -f replace_std replace_sys
