#ifndef __COMMON_H__
#define __COMMON_H__

#include <semaphore.h>

#ifdef POSIX
    #define PROJECT_IDENTIFIER "SHARED"
    #define SEM_QUEUE_FNAME "/sem_queue"
    #define SEM_CHAIRS_FNAME "/sem_chairs"
    #define SEM_BARBERS_FNAME "/sem_barbers"
    #define SEM_BUFFER_MUTEX_FNAME "/sem_mutex"
#endif
#ifdef SYSTEM_V 
    #define PROJECT_IDENTIFIER getenv("HOME")
    #define SEM_QUEUE_FNAME "0"
    #define SEM_CHAIRS_FNAME "1"
    #define SEM_BARBERS_FNAME "2"
    #define SEM_BUFFER_MUTEX_FNAME "3"
#endif

// Simulation config
#define BARBER_TOTAL 3      // M
#define CHAIR_TOTAL 2       // N
#define QUEUE_SIZE 5    // P
#define CUSTOMERS_TOTAL 6
#define BUFFER_SIZE 4096

#endif