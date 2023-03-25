#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIGNAL SIGUSR1
#define MAX_CALLS 4

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_CYAN  "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BOLD  "\x1b[1m"

int call_id;
int call_depth;

typedef void (*handler_t)(int, siginfo_t*, void*);

void set_action(struct sigaction action, handler_t handler, int flag) {
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = flag;
    sigaction(SIGNAL, &action, NULL);
    call_id = 0;
    call_depth = 0;
}

void depth_handler(int signo, siginfo_t* info, void* context) {
    printf("[SIGTEST] (+) call id: %d, call depth: %d\n", call_id, call_depth);
    
    call_id++;
    call_depth++;
    if (call_id < MAX_CALLS)
        kill(getpid(), SIGNAL);
    call_depth--;

    printf("[SIGTEST] (-) call id: %d, call depth: %d\n", call_id-1, call_depth);
}

void info_handler(int signo, siginfo_t* info, void* context) {
    printf(ANSI_COLOR_BOLD "[SIGTEST]    Signal number:%s %d%s\n", ANSI_COLOR_RESET ANSI_COLOR_CYAN, info->si_signo, ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BOLD "[SIGTEST]    PID:%s %d%s\n", ANSI_COLOR_RESET ANSI_COLOR_CYAN, info->si_pid, ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BOLD "[SIGTEST]    UID:%s %d%s\n", ANSI_COLOR_RESET ANSI_COLOR_CYAN, info->si_uid, ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BOLD "[SIGTEST]    POSIX timer ID:%s %d%s\n", ANSI_COLOR_RESET ANSI_COLOR_CYAN, info->si_timerid, ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BOLD "[SIGTEST]    Exit value / signal:%s %x%s\n", ANSI_COLOR_RESET ANSI_COLOR_CYAN, info->si_status, ANSI_COLOR_RESET);
}

void testNODEFER(struct sigaction act) {
    set_action(act, depth_handler, SA_NODEFER);
    kill(getpid(), SIGNAL);
}

void testSIGINFO(struct sigaction act) {
    printf(ANSI_COLOR_BOLD "[SIGTEST]  SELF:\n" ANSI_COLOR_RESET);
    set_action(act, info_handler, SA_SIGINFO);
    kill(getpid(), SIGNAL);
    printf("\n");

    printf(ANSI_COLOR_BOLD "[SIGTEST]  CHILD:\n" ANSI_COLOR_RESET);
    if (fork() == 0) {
        kill(getpid(), SIGNAL);
        exit(0);
    }
    else
        wait(NULL);
    printf("\n");

    printf(ANSI_COLOR_BOLD "[SIGTEST]  CUSTOM:\n" ANSI_COLOR_RESET);
    set_action(act, info_handler, SA_SIGINFO);
    sigval_t sig_val = {0xBADC0DE};
    sigqueue(getpid(), SIGNAL, sig_val);
}

void testRESETHAND(struct sigaction act) {
    set_action(act, depth_handler, SA_RESETHAND);
    kill(getpid(), SIGNAL);
    kill(getpid(), SIGNAL);
}

int main(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "[SIGTEST] Invalid arguments.\n");
        exit(1);
    }

    struct sigaction action;

    printf(ANSI_COLOR_GREEN ANSI_COLOR_BOLD "[SIGTEST] SIGINFO:\n" ANSI_COLOR_RESET);
    testSIGINFO(action);
    printf("\n");

    printf(ANSI_COLOR_GREEN ANSI_COLOR_BOLD "[SIGTEST] NODEFER:\n" ANSI_COLOR_RESET);
    testNODEFER(action);
    printf("\n");

    printf(ANSI_COLOR_GREEN ANSI_COLOR_BOLD "[SIGTEST] RESETHAND:\n" ANSI_COLOR_RESET);
    testRESETHAND(action);

    return 0;
}