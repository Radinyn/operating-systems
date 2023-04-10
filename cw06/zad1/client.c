#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

#include "common.h"

key_t qKey;
int qID;
int squeueID;
int client_id;

int handle_init();
void handle_stop();
void handle_server_message();
void handle_LIST();
void handle_2ALL(char *message);
void handle_2ONE(int other_id, char *message);

int main() {
    srand(time(NULL));

    qKey = ftok(HOME_PATH, rand() % 255 + 1);
    qID = msgget(qKey, IPC_CREAT | 0666);
    key_t sKey = ftok(HOME_PATH, SERVER_ID);
    squeueID = msgget(sKey, 0);
    client_id = handle_init();

    signal(SIGINT, handle_stop);

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
            handle_LIST();
        } else if (strcmp(curr_cmd, "2ALL") == 0) {
            curr_cmd = strtok(NULL, " ");
            char *message = curr_cmd;
            handle_2ALL(message);
        } else if (strcmp(curr_cmd, "2ONE") == 0) {
            curr_cmd = strtok(NULL, " ");
            int destinationID = atoi(curr_cmd);
            curr_cmd = strtok(NULL, " ");
            char *message = curr_cmd;
            handle_2ONE(destinationID, message);
        } else if (strcmp(curr_cmd, "STOP") == 0) {
            handle_stop();
        } else {
            printf("Command not recognized!\n");
        }
    }
}

int handle_init() {
    time_t my_time = time(NULL);
    MessageBuffer *pMessageBuffer = malloc(sizeof(MessageBuffer));

    pMessageBuffer->time_struct = *localtime(&my_time);
    pMessageBuffer->type = INIT;
    pMessageBuffer->queue_key = qKey;

    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
    msgrcv(qID, pMessageBuffer, MSG_SIZE, 0, 0);

    int client_id = pMessageBuffer->client_id;
    if (client_id == -1) {
        printf("Client limit had been reached, leaving..\n");
        exit(0);
    }

    return client_id;
}

void handle_LIST() {
    time_t my_time = time(NULL);
    MessageBuffer *pMessageBuffer = malloc(sizeof(MessageBuffer));

    pMessageBuffer->time_struct = *localtime(&my_time);
    pMessageBuffer->type = LIST;
    pMessageBuffer->client_id = client_id;

    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
    msgrcv(qID, pMessageBuffer, MSG_SIZE, 0, 0);
    printf("%s\n", pMessageBuffer->content);
}

void handle_2ALL(char *message) {
    time_t my_time = time(NULL);
    MessageBuffer *pMessageBuffer = malloc(sizeof(MessageBuffer));

    pMessageBuffer->time_struct = *localtime(&my_time);
    pMessageBuffer->type = TALL;
    strcpy(pMessageBuffer->content, message);

    pMessageBuffer->client_id = client_id;
    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
}

void handle_2ONE(int other_id, char *message) {
    time_t my_time = time(NULL);
    MessageBuffer *pMessageBuffer = malloc(sizeof(MessageBuffer));

    pMessageBuffer->time_struct = *localtime(&my_time);
    pMessageBuffer->type = TONE;
    strcpy(pMessageBuffer->content, message);

    pMessageBuffer->client_id = client_id;
    pMessageBuffer->other_id = other_id;
    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
}

void handle_stop() {
    time_t my_time = time(NULL);
    MessageBuffer *pMessageBuffer = malloc(sizeof(MessageBuffer));

    pMessageBuffer->time_struct = *localtime(&my_time);
    pMessageBuffer->type = STOP;
    pMessageBuffer->client_id = client_id;

    msgsnd(squeueID, pMessageBuffer, MSG_SIZE, 0);
    msgctl(qID, IPC_RMID, NULL);
    exit(0);
}

void handle_server_message() {
    MessageBuffer *msg_rcv = malloc(sizeof(MessageBuffer));
    while (msgrcv(qID, msg_rcv, MSG_SIZE, 0, IPC_NOWAIT) >= 0) {
        if (msg_rcv->type == STOP) {
            printf("Received stop message, leaving..\n");
            handle_stop();
        } else {
            struct tm my_time = msg_rcv->time_struct;
            printf("Msg from: %d has been sent at %02d:%02d:%02d:\n%s\n",
                   msg_rcv->client_id,
                   my_time.tm_hour,
                   my_time.tm_min,
                   my_time.tm_sec,
                   msg_rcv->content);
        }
    }
}