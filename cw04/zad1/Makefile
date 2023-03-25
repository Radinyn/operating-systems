SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3

all: 
	make fork
	make exec

fork: signals.c
	$(CC) $(CFLAGS) signals.c -o signals_fork

exec: signals.c
	$(CC) $(CFLAGS) signals.c -o signals_exec -D EXEC

report:
	make all

	echo "" > fork.txt
	echo "" > exec.txt

	echo ">>> Ignore:" >> fork.txt
	./signals_fork ignore >> fork.txt
	echo "" >> fork.txt

	echo ">>> Handler:" >> fork.txt
	./signals_fork handler >> fork.txt
	echo "" >> fork.txt

	echo ">>> Mask:" >> fork.txt
	./signals_fork mask >> fork.txt
	echo "" >> fork.txt

	echo ">>> Pending:" >> fork.txt
	./signals_fork pending >> fork.txt
	echo "" >> fork.txt

	echo ">>> Ignore:" >> exec.txt
	./signals_exec ignore >> exec.txt
	echo "" >> exec.txt

	echo ">>> Mask:" >> exec.txt
	./signals_exec mask >> exec.txt
	echo "" >> exec.txt

	echo ">>> Pending:" >> exec.txt
	./signals_exec pending >> exec.txt
	echo "" >> exec.txt


clean:
	rm -f signals_fork signals_exec
