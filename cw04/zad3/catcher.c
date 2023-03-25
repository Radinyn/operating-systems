#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define SIGNAL SIGUSR1

typedef enum {
    NUMBERS   = 1,
    TIME_ONCE = 2,
    REQUESTS  = 3,
    TIME_LOOP = 4,
    FINISH    = 5,
} CATCHER_STATE;

CATCHER_STATE state = NUMBERS;
bool done = true;
int request_count = 0;

void print_current_time() {
    static time_t raw_time;
    static struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    printf("[CATCHER] Current timestamp: %s", asctime(time_info));
}

void numbers() {
    for (int i = 1; i <= 100; i++)
        printf("[CATCHER] %i\n", i);
}

void time_once() {
    print_current_time();
}

void requests() {
    printf("[CATCHER] Number of valid requests: %d\n", request_count);
}

void time_loop(double dt) {
    static double acc = 1.0;

    acc += dt;
    if (acc >= 1.0) {
        print_current_time();
        acc = 0.0;
    }
}

void finish() {
    printf("[CATCHER] Ending.\n");
    fflush(NULL);
    exit(0);
}

void handler(int signo, siginfo_t* info, void* context) {
    int pid = info->si_pid;
    int request = info->si_status;

    if (request < 1 || request > 5) {
        fprintf(stderr, "[CATCHER] Invalid request (%d).\n", request);
    } else {
        request_count += 1;
        state = (CATCHER_STATE) request;
        done = false;
    }

    kill(pid, SIGNAL);
}

double maxd(double x, double y) {
    return x > y ? x : y;
}

int main() {

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGNAL, &action, NULL);
    
    clock_t begin = clock();
    clock_t end = clock();

    printf("[CATCHER] Starting, PID=(%d)\n", getpid());
    while (true) {
        double dt = maxd(0.0, (double)(end-begin)/CLOCKS_PER_SEC);

        begin = clock();

        if (done)
            continue;

        switch (state) {
            case NUMBERS:   numbers();     break;
            case TIME_ONCE: time_once();   break;
            case REQUESTS:  requests();    break;
            case TIME_LOOP: time_loop(dt); break;
            case FINISH:    finish();      break;
        }

        if (state != TIME_LOOP)
            done = true;
    
        end = clock();
    }

    return 0;
}

