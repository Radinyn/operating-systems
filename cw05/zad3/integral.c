#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_PATH "/tmp/integral_queue"
#define BUFF_SIZE 2048
char write_buff[BUFF_SIZE] = "";
char read_buff[BUFF_SIZE] = "";
int _;

char arg2[BUFF_SIZE];
char arg3[BUFF_SIZE];

struct timespec timespec_diff(struct timespec start, struct timespec end) {
    struct timespec out;

    if ((end.tv_nsec-start.tv_nsec)<0) {
            out.tv_sec = end.tv_sec-start.tv_sec-1;
            out.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else {
            out.tv_sec = end.tv_sec-start.tv_sec;
            out.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return out;
}


int main(int argc, char** argv) {
    double dx = strtod(argv[1], NULL);
    int n = atoi(argv[2]);
    double inv_n = 1.0/n;

    struct timespec timespec_buff_start, timespec_buff_end;
    clock_gettime(CLOCK_REALTIME, &timespec_buff_start);

    mkfifo(FIFO_PATH, 0666);

    for (int i = 0; i < n; i++)
        if (!fork()) {
            snprintf(arg2, BUFF_SIZE, "%lf", i*inv_n);
            snprintf(arg3, BUFF_SIZE, "%lf", (i+1)*inv_n);
            execl("./worker", "worker", argv[1], arg2, arg3, NULL);
        }

    double result = 0.0;
    
    int fifo = open(FIFO_PATH, O_RDONLY);
    int already_read = 0;
    while (already_read < n) {
        size_t size = read(fifo, read_buff, BUFF_SIZE);
        read_buff[size] = 0;

        char delim[] = "\n";
        char* token;

        token = strtok(read_buff, delim);
        for (;token; token = strtok(NULL, delim)) {
            result += strtod(token, NULL);
            already_read++;
        }
    }
    close(fifo);

    remove(FIFO_PATH);

    clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
    struct timespec diff = timespec_diff(timespec_buff_start, timespec_buff_end);
    printf("RESULT: %lf\nN: %d\nDX: %.17f\nTIME: %lds %ldns\n\n", result, n, dx, diff.tv_sec, diff.tv_nsec);

    return 0;
}

