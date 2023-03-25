#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#define SIGNAL SIGUSR1

int is_parent = 1;

typedef enum {
    INVALID,
    IGNORE,
    HANDLER,
    MASK, 
    PENDING,
} TASK_TYPE;

TASK_TYPE str_to_task_type(const char* str) {
    if (strcmp(str, "ignore")  == 0) return IGNORE;
    if (strcmp(str, "handler") == 0) return HANDLER;
    if (strcmp(str, "mask")    == 0) return MASK;
    if (strcmp(str, "pending") == 0) return PENDING;
    return INVALID;
}

void signal_handler(int signo) {
    printf("[SIGNALS] (%d) Received signal.\n", getpid());
}

void print_is_pending() {
    sigset_t sigset;
    sigpending(&sigset);
    if (sigismember(&sigset, SIGNAL))
        printf("[SIGNALS] (%d) Signal pending.\n", getpid());
    else
        printf("[SIGNALS] (%d) Signal not pending.\n", getpid());
}

void block_signal() {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGNAL);
    sigprocmask(SIG_BLOCK, &action.sa_mask, NULL);
}

void raise_signal() {
    printf("[SIGNALS] (%d) Raising signal.\n", getpid());
    raise(SIGNAL);
}

TASK_TYPE handle_input(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "[SIGNALS] Invalid argument count.\n");
        exit(1);
    }

    #ifdef EXEC
    if (argc == 3 && strcmp(argv[2], "child") != 0) {
        fprintf(stderr, "[SIGNALS] argv[3] should never be set manually.\n");
        exit(2);
    }
    #else
    if (argc == 3) {
        fprintf(stderr, "[SIGNALS] Invalid argument count. Only EXEC version takes in 3 arguments.\n");
        exit(2);
    }
    #endif

    TASK_TYPE task_type = INVALID;
    if ((task_type = str_to_task_type(argv[1])) == INVALID) {
        fprintf(stderr, "[SIGNALS] Invalid task_type (%s).\n", argv[2]);
        exit(3);
    }

    #ifdef EXEC
    if (task_type == HANDLER) {
        fprintf(stderr, "[SIGNALS] Handler is not a valid task_type for EXEC version.\n");
        exit(4);
    }

    if (argc == 3) {
        is_parent = 0;
    }
    #endif

    return task_type;
}

void process(TASK_TYPE task_type) {
    switch (task_type) {
        case IGNORE:
            signal(SIGNAL, SIG_IGN);
            raise_signal();
            break;

        case HANDLER:
            signal(SIGNAL, signal_handler);
            raise_signal();
            break;

        case MASK:
        case PENDING:
            signal(SIGNAL, signal_handler);

            if (is_parent) {
                block_signal();
                raise_signal();
            }
            
            if (task_type == PENDING)
                print_is_pending();
            break;

        case INVALID:
            break;
    }
}

int main(int argc, char** argv) {
    TASK_TYPE task_type = handle_input(argc, argv);

    process(task_type);
    fflush(NULL);

    #ifdef EXEC
        if (is_parent)
            execl(argv[0], argv[0], argv[1], "child", NULL);
    #else
        is_parent = fork();
        if (!is_parent)
            process(task_type);
        else
            wait(NULL);
    #endif

    fflush(NULL);
    return 0;
}