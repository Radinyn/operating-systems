#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "[SPAWN] Invalid arguments\n");
        return 1;
    }

    int n = atoi(argv[1]);

    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            printf("PARENT: %d | OWN: %d\n", getppid(), getpid());
            return 0;
        }
    }

    while (wait(NULL) > 0);
    printf("ARGV[1] = %d\n", n);

    return 0;
}
