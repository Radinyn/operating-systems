#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define BUFF_SIZE 256
char write_buff[BUFF_SIZE] = "";
char read_buff[BUFF_SIZE] = "";
int _;

double f(double x) {
    return 4/(x*x+1);
} 

double integral(double a, double b, double dx) {
    double acc = 0.0;
    for (double x = a; x < b; x += dx)
        acc += f(x)*dx;
    return acc;
}

double sum(double* arr, double n) {
    double acc = 0.0;
    for (int i = 0; i < n; i++)
        acc += arr[i];
    return acc;
}

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

    int* pipes = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++) {
        int fd[2];
        _ = pipe(fd);
        if (fork()) { // parent
            close(fd[1]); 
            pipes[i] = fd[0];
        } else { // child
            close(fd[0]);
            double out = integral(i*inv_n, (i+1)*inv_n, dx);
            size_t size = snprintf(write_buff, BUFF_SIZE, "%lf", out);
            _ = write(fd[1], write_buff, size);
            exit(0);
        }
    }

    while (wait(NULL) > 0);

    double result = 0.0;
    for (int i = 0; i < n; i++) {
        size_t size = read(pipes[i], read_buff, BUFF_SIZE);
        read_buff[size] = 0;
        result += strtod(read_buff, NULL);
    }

    free(pipes);

    clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
    struct timespec diff = timespec_diff(timespec_buff_start, timespec_buff_end);
    printf("RESULT: %lf\nN: %d\nDX: %.17f\nTIME: %lds %ldns\n\n", result, n, dx, diff.tv_sec, diff.tv_nsec);

    return 0;
}

