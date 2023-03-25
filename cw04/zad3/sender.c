#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define SIGNAL SIGUSR1
#define STATES_START 2

volatile int ack = STATES_START;

void handler(int signo) {
    ack++;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "[SENDER] Invalid arguments.\n");
        exit(1);
    }

    int state;
    bool failed = false;

    int catcher_pid = atoi(argv[1]);
    for (int i = STATES_START; i < argc; i++) {
        failed = false;
        state = atoi(argv[i]);

        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = handler;
        sigaction(SIGNAL, &action, NULL);

        sigval_t sig_val = {state};
        sigqueue(catcher_pid, SIGNAL, sig_val);
        printf("[SENDER] Sent state (%d).\n", state);

        time_t start = clock();
        while (ack <= i) {
            if (clock()-start > CLOCKS_PER_SEC*5) {
                printf("[SENDER] No confirmation, retrying.\n");
                failed = true;
                i--;
                break;
            }
        }

        if (!failed)
            printf("[SENDER] Received comfirmation.\n");
    }

    return 0;
}