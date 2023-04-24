#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>

#include "common.h"
#include "shared_memory.h"
#include "queue.h"
#include "sem.h"

#ifdef POSIX
    #define BARBER_EXEC "./barber_POSIX"
    #define CLIENT_EXEC "./client_POSIX"
#endif
#ifdef SYSTEM_V
    #define BARBER_EXEC "./barber_SYSTEM_V"
    #define CLIENT_EXEC "./client_SYSTEM_V"
#endif

static Semaphore sem_queue;
static Semaphore sem_chairs;
static Semaphore sem_barbers;
static Semaphore buffer_mutex;

void close_semaphores(void);
void unlink_semaphores(void);
void create_semaphores(void);

int main(void) {
    printf("[SIMULATION] barbers total: %d, chairs total: %d, queue size: %d, customers waiting: %d\n",
            BARBER_TOTAL,
            CHAIR_TOTAL,
            QUEUE_SIZE,
            CUSTOMERS_TOTAL);
    fflush(stdout);

    char *shared = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if(shared == NULL) {
        exit(EXIT_FAILURE);
    }
    shared[0] = '\0';

    unlink_semaphores();
    create_semaphores();

    for(int i=0;i<BARBER_TOTAL;++i)
        if (fork() == 0)
            execl(BARBER_EXEC, BARBER_EXEC, NULL);
    printf("[SIMULATION] Spawned all barbers.\n");
    fflush(stdout);


    for(int i=0;i<CUSTOMERS_TOTAL;++i)
        if (fork() == 0)
            execl(CLIENT_EXEC, CLIENT_EXEC, NULL);
    printf("[SIMULATION] Spawned all customers.\n");
    fflush(stdout);

    while(wait(NULL) > 0);

    if (!destroy_shared_memory(PROJECT_IDENTIFIER)) {
        fprintf(stderr, "[ERROR] Failed to release shared memory.\n");
        exit(EXIT_FAILURE);
    }
    close_semaphores();
    printf("[SIMULATION] Simulation finished.\n");
    fflush(stdout);
    return EXIT_SUCCESS;
}


void close_semaphores(void) {
    close_semaphore(sem_queue);
    close_semaphore(sem_chairs);
    close_semaphore(sem_barbers);
    close_semaphore(buffer_mutex);
}

void unlink_semaphores(void) {
    unlink_semaphore(SEM_QUEUE_FNAME);
    unlink_semaphore(SEM_CHAIRS_FNAME);
    unlink_semaphore(SEM_BARBERS_FNAME);
    unlink_semaphore(SEM_BUFFER_MUTEX_FNAME);
}

void create_semaphores(void) {
    sem_queue =  create_semaphore(SEM_QUEUE_FNAME, CHAIR_TOTAL);
    sem_chairs =  create_semaphore(SEM_CHAIRS_FNAME, 0);
    sem_barbers =  create_semaphore(SEM_BARBERS_FNAME, 0);
    buffer_mutex =  create_semaphore(SEM_BUFFER_MUTEX_FNAME, 1);
}