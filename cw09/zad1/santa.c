#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdbool.h>

#define REINDEER_TOTAL 9
#define ELVES_TOTAL 10
#define ELVES_REQUIRED 3

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    unsigned int elves, reindeer;
} Monitor;

static Monitor monitor = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .condition = PTHREAD_COND_INITIALIZER,
    .reindeer = 0,
    .elves = 0
};
static pthread_barrier_t reindeer_barrier, elves_barrier;
    
pid_t get_thread_id() {
    return syscall(__NR_gettid); 
}

void *reindeer_routine(void *arg) {
    while (true) {
        printf("[REINDEER] Reindeer [%d] is on vacation \n", get_thread_id());
        sleep(5 + (rand() % 6));

        pthread_mutex_lock(&monitor.mutex);
        monitor.reindeer++;
        if (monitor.reindeer == REINDEER_TOTAL) {
            printf("[REINDEER] Reindeer [%d] wakes up Santa.\n", get_thread_id());
        }
        else {
            printf("[REINDEER] Reindeer [%d] came back from vacation\n", get_thread_id());
        }
        pthread_cond_signal(&monitor.condition);
        pthread_mutex_unlock(&monitor.mutex);

        pthread_barrier_wait(&reindeer_barrier);
    }
    return NULL;
}

void *elf_routine(void *arg) {
    while(true) {
        sleep(2 + (rand() % 4));

        pthread_mutex_lock(&monitor.mutex);
        if (monitor.elves < ELVES_REQUIRED ) {
            monitor.elves += 1;
            if (monitor.elves == ELVES_REQUIRED) {
                printf("[ELF] [%d] wakes up santa\n", get_thread_id());
            }
            else {
                printf("[ELF] Elf [%d] is waiting for santa.\n", get_thread_id());
            }
            pthread_cond_signal(&monitor.condition);
        }
        else {
            printf("[ELF] Elf [%d] solves the problem on their own.\n", get_thread_id());
            pthread_mutex_unlock(&monitor.mutex);
            continue;
        }
        pthread_mutex_unlock(&monitor.mutex);
        pthread_barrier_wait(&elves_barrier);

    }
    return NULL;
}

int main(int argc, char** argv) {
    pthread_barrier_init(&reindeer_barrier, NULL, REINDEER_TOTAL + 1);
    pthread_barrier_init(&elves_barrier, NULL, ELVES_REQUIRED + 1);

    pthread_t reindeer_ids[REINDEER_TOTAL];
    for(int i=0;i<REINDEER_TOTAL;i++) {
        pthread_create(&reindeer_ids[i], NULL, reindeer_routine, NULL);
    }

    pthread_t elf_ids[ELVES_TOTAL];
    for (int i=0;i<ELVES_TOTAL;i++) {
        pthread_create(&elf_ids[i], NULL, elf_routine, NULL);
    }

    while(true) {
        pthread_mutex_lock(&monitor.mutex);
        while(monitor.reindeer < REINDEER_TOTAL && monitor.elves < ELVES_REQUIRED) {
            pthread_cond_wait(&monitor.condition, &monitor.mutex);
        }
        if (monitor.reindeer == REINDEER_TOTAL) {
            printf("[SANTA] Santa is awake! Delivering gifts.\n");
            sleep(2 + rand() % 3);
            monitor.reindeer = 0;
            pthread_barrier_wait(&reindeer_barrier);
        }
        else if (monitor.elves == ELVES_REQUIRED) {
            printf("[SANTA] Santa is awake! Helping elves.\n");
            sleep(1 + rand() % 2);
            monitor.elves = 0;
            pthread_barrier_wait(&elves_barrier);
        }
        else {
            printf("Something went wrong!\n");
        }
        pthread_mutex_unlock(&monitor.mutex);
    }
    return EXIT_SUCCESS;
}
