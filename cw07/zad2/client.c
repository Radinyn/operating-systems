#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>

#include "shared_memory.h"
#include "queue.h"
#include "common.h"
#include "sem.h"

static Semaphore sem_queue;
static Semaphore sem_chairs;
static Semaphore sem_barbers;
static Semaphore buffer_mutex;

void open_semaphores();
char random_haircut();

int main(void) {
    srand(time(NULL) + getpid());

    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if (queue == NULL) {
        fprintf(stderr, "[ERROR] Can't open queue.\n");
        exit(EXIT_FAILURE);
    }

    open_semaphores();

    if (strlen(queue) >= QUEUE_SIZE) {
        printf("\t[CLIENT-%d] Queue full. Leaving.\n", getpid());
        fflush(stdout);
    }
    
    aquire(sem_queue);

    aquire(buffer_mutex);
    char haircut = random_haircut();
    printf("\t[CLIENT-%d] New client with haircut no. %d \n", getpid(), haircut);
    fflush(stdout);
    queue_push(queue, haircut);
    release(buffer_mutex);
    
    release(sem_barbers);
    aquire(sem_chairs);

    printf("\t[CLIENT-%d] Client done.\n", getpid());
    fflush(stdout);

    detach_shared_memory(queue);

    return EXIT_SUCCESS;
}

void open_semaphores()
{
    sem_queue = open_semaphore(SEM_QUEUE_FNAME);
    sem_chairs = open_semaphore(SEM_CHAIRS_FNAME);
    sem_barbers = open_semaphore(SEM_BARBERS_FNAME);
    buffer_mutex = open_semaphore(SEM_BUFFER_MUTEX_FNAME);
}

char random_haircut() {
    return (char) (rand() % 128);
}