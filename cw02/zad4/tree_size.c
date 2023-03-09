#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>
#include <errno.h>
#include <string.h>

long long total_size = 0;

int file_callback(const char *fpath, const struct stat *sb, int typeflag) {
    if (typeflag != FTW_D) {
        struct stat stats;
        stat(fpath, &stats);
        printf("%s %ld\n", fpath, stats.st_size);
        total_size += stats.st_size;
    }
    return 0;
}


int main(int argc, char** argv)  {
    if (argc != 2) {
        fprintf(stderr, "[TREE SIZE] Invalid arguments\n");
        return 1;
    }

    char* input_dirname = argv[1];
    int error = ftw(input_dirname, file_callback, 1);

    if (error == -1) {
        fprintf(stderr, "[TREE SIZE] FTW error has occured (%s)\n", strerror(errno));
        return 2;
    }

    printf("TOTAL %lld\n", total_size);

    return 0;
}
