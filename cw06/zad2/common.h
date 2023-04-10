#ifndef LAB6_COMMON_H
#define LAB6_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>


#define LENN 3
#define SQUEUE "/SERVER"
#define MAX_NO_CLIENTS 10
#define MAX_MESSAGE_SIZE 512


typedef struct message_buffer{
    int client_id;
    int other_id;
    long type;
    char content[MAX_MESSAGE_SIZE];
    struct tm time_struct;
} message_buffer;

typedef enum MessageType {
    INIT = 1,
    LIST = 2,
    TONE = 3,
    TALL = 4,
    STOP = 5
} MessageType;


const int MSG_SIZE = sizeof(message_buffer);


char randomC() {
    return rand() % ('Z' - 'A' + 1) + 'A';
}

mqd_t create_queue(const char* name) {
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_NO_CLIENTS;
    attr.mq_msgsize = MSG_SIZE;
    return mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
}

#endif 
