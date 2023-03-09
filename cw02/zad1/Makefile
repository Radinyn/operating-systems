SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all: 
	make std
	make sys

std: replace.c
	$(CC) $(CFLAGS) replace.c -o replace_std

sys: replace.c
	$(CC) $(CFLAGS) replace.c -o replace_sys -D SYS

report:
	make all

	rm -f pomiar_zad_2_1.txt pomiar_zad_2_1024.txt

	i=1 ; while [[ $$i -le 10 ]] ; do \
		head -c 1048576 </dev/urandom >test.in ; \
		./replace_sys a b test.in tmp_sys >> pomiar_zad_1_sys.txt ; \
		./replace_std a b test.in tmp_std >> pomiar_zad_1_std.txt ; \
		((i = i + 1)) ; \
	done

	rm -f test.in tmp_std tmp_sys


clean:
	rm -f replace_std replace_sys
