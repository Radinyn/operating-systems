#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

#include "common.h"

int cID;
char qName[LENN];
mqd_t qDesc;
mqd_t sQueueDesc;

int manage_init();
void manage_stop();
void handle_server_message();
void get_new_name();
void manage_LIST();
void manage_2ALL(char *message);
void manage_2ONE(int other_id, char *message);

int main() {
    srand(time(NULL));

    get_new_name();
    qDesc = create_queue(qName);
    sQueueDesc = mq_open(SQUEUE, O_RDWR);
    cID = manage_init();

    signal(SIGINT, manage_stop);

    size_t len = 0;
    ssize_t read;
    char *command = NULL;

    while (1) {
        printf("User command: ");
        read = getline(&command, &len, stdin);
        command[read - 1] = '\0';

        handle_server_message();

        if (strcmp(command, "") == 0) {
            continue;
        }

        char *curr_cmd = strtok(command, " ");
        if (strcmp(curr_cmd, "LIST") == 0) {
            manage_LIST();
        } else if (strcmp(curr_cmd, "2ALL") == 0) {
            curr_cmd = strtok(NULL, " ");
            char *message = curr_cmd;
            manage_2ALL(message);
        } else if (strcmp(curr_cmd, "2ONE") == 0) {
            curr_cmd = strtok(NULL, " ");
            int destinationID = atoi(curr_cmd);
            curr_cmd = strtok(NULL, " ");
            char *message = curr_cmd;
            manage_2ONE(destinationID, message);
        } else if (strcmp(curr_cmd, "STOP") == 0) {
            manage_stop();
        } else {
            printf("Command not recognized!\n");
        }
    }
}

int manage_init() {
    time_t my_time = time(NULL);
    message_buffer messageBuffer;

    messageBuffer.time_struct = *localtime(&my_time);
    messageBuffer.type = INIT;

    strcpy(messageBuffer.content, qName);
    mq_send(sQueueDesc, (char *) &messageBuffer, MSG_SIZE, 0);
    mq_receive(qDesc, (char *) &messageBuffer, MSG_SIZE, NULL);

    cID = messageBuffer.client_id;
    if (cID == -1) {
        printf("No room for more clients, leaving..\n");
        exit(0);
    }

    return cID;
}

void manage_stop() {
    time_t my_time = time(NULL);
    message_buffer messageBuffer;

    messageBuffer.time_struct = *localtime(&my_time);
    messageBuffer.type = STOP;
    messageBuffer.client_id = cID;

    mq_send(sQueueDesc, (char *) &messageBuffer, MSG_SIZE, 0);
    mq_close(sQueueDesc);
    exit(0);
}

void handle_server_message() {
    struct timespec my_time;
    message_buffer messageBuffer;

    clock_gettime(CLOCK_REALTIME, &my_time);
    my_time.tv_sec += 0.1;
    while (mq_timedreceive(qDesc, (char *) &messageBuffer,
                           MSG_SIZE, NULL, &my_time) != -1) {
        
        if (messageBuffer.type == STOP) {
            manage_stop();
        } else {
            printf("%s\n", messageBuffer.content);
        }
    }
}

void get_new_name() {
    qName[0] = '/';

    for (int i = 1; i < LENN; i++) {
        qName[i] = randomC();
    }
}

void manage_LIST() {
    time_t my_time = time(NULL);
    message_buffer messageBuffer;

    messageBuffer.time_struct = *localtime(&my_time);
    messageBuffer.type = LIST;
    messageBuffer.client_id = cID;

    mq_send(sQueueDesc, (char *) &messageBuffer, MSG_SIZE, 0);
    mq_receive(qDesc, (char *) &messageBuffer, MSG_SIZE, NULL);
    printf("%s\n", messageBuffer.content);
}

void manage_2ALL(char *message) {
    time_t my_time = time(NULL);
    message_buffer messageBuffer;

    messageBuffer.time_struct = *localtime(&my_time);
    messageBuffer.type = TALL;
    strcpy(messageBuffer.content, message);

    messageBuffer.client_id = cID;
    mq_send(sQueueDesc, (char *) &messageBuffer, MSG_SIZE, 0);
}

void manage_2ONE(int other_id, char *message) {
    time_t my_time = time(NULL);
    message_buffer messageBuffer;

    messageBuffer.time_struct = *localtime(&my_time);
    messageBuffer.type = TONE;
    strcpy(messageBuffer.content, message);

    messageBuffer.client_id = cID;
    messageBuffer.other_id = other_id;
    mq_send(sQueueDesc, (char *) &messageBuffer, MSG_SIZE, 0);
}
