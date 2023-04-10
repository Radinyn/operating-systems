#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>


#define HOME_PATH getenv("HOME")
#define SERVER_ID 1
#define MAX_NO_CLIENTS 10
#define MAX_SIZE_MESSAGE 512


typedef struct MessageBuffer {
    key_t queue_key;
    int client_id;
    int other_id;
    long type;
    char content[MAX_SIZE_MESSAGE];
    struct tm time_struct;
} MessageBuffer;

typedef enum MessageType {
    INIT = 1,
    LIST = 2,
    TONE = 3,
    TALL = 4,
    STOP = 5
} MessageType;


const int MSG_SIZE = sizeof(MessageBuffer);

#endif
