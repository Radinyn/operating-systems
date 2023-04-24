#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "common.h"
#include "shared_memory.h"
#include "queue.h"
#include "sem.h"

#define HAIRCUT_TIME 1000
#define TIMEOUT 1000000

static Semaphore sem_queue;
static Semaphore sem_chairs;
static Semaphore sem_barbers;
static Semaphore buffer_mutex;

void open_semaphores();

int main(void)
{
    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if (queue == NULL)
    {
        fprintf(stderr, "[ERROR] Can't open queue.\n");
        exit(EXIT_FAILURE);
    }
    open_semaphores();

    printf("\t[BARBER-%d] Spawned\n", getpid());
    fflush(stdout);

    while (true)
    {

        aquire(sem_barbers);

        release(buffer_mutex);
        char haircut = queue_pop(queue);
        release(buffer_mutex);

        printf("\t[BARBER-%d] Processing hairuct no. %d\n", getpid(), haircut);
        fflush(stdout);

        usleep(HAIRCUT_TIME);

        printf("\t[BARBER-%d] Done with hairuct no. %d\n", getpid(), haircut);
        fflush(stdout);

        release(sem_chairs);
        release(sem_queue);

        if (queue_empty(queue))
        {
            usleep(TIMEOUT);
            if (queue_empty(queue))
                break;
        }
    }
    printf("\t[BARBER-%d] Queue empty. Closing. \n", getpid());
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