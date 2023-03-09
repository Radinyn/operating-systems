SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all: 
	make single BLOCK_SIZE=1
	make single BLOCK_SIZE=8
	make single BLOCK_SIZE=1024

single: 
	$(CC) $(CFLAGS) reverse.c -o reverse_$(BLOCK_SIZE) -D BLOCK_SIZE=$(BLOCK_SIZE)

report:
	make all

	rm -f pomiar_zad_2_1.txt pomiar_zad_2_1024.txt

	i=1 ; while [[ $$i -le 10 ]] ; do \
		head -c 1048576 </dev/urandom >test.in ; \
		./reverse_1 test.in tmp1 >> pomiar_zad_2_1.txt ; \
		./reverse_1024 test.in tmp1024 >> pomiar_zad_2_1024.txt ; \
		((i = i + 1)) ; \
	done

	rm -f test.in tmp1 tmp1024


clean:
	rm -f reverse_* test.in
