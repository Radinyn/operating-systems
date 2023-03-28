#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define FIFO_PATH "/tmp/integral_queue"
#define BUFF_SIZE 256
char write_buff[BUFF_SIZE] = "";
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

int main(int argc, char** argv) {
    double dx = strtod(argv[1], NULL);
    double a = strtod(argv[2], NULL);
    double b = strtod(argv[3], NULL);

    double out = integral(a, b, dx);
    size_t size = snprintf(write_buff, BUFF_SIZE, "%lf\n", out);

    int fifo = open(FIFO_PATH, O_WRONLY);
    _ = write(fifo, write_buff, size);
    close(fifo);

    return 0;
}