SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -O3
OTHER=sem.c shared_memory.c queue.c

all: 
	make simulation STANDARD=POSIX
	make simulation STANDARD=SYSTEM_V

simulation: client barber
	$(CC) $(CFLAGS) $(SOURCES) simulation.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o simulation_$(STANDARD)

client:
	$(CC) $(CFLAGS) $(SOURCES) client.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o client_$(STANDARD)

barber:
	$(CC) $(CFLAGS) $(SOURCES) barber.c $(OTHER) -D$(STANDARD) -lrt -lpthread -o barber_$(STANDARD)

clean:
	rm -rf barber_* client_* simulation_*