#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char** argv)  {
    if (argc != 2) {
        fprintf(stderr, "[DIR SIZE] Invalid arguments\n");
        return 1;
    }

    char* input_dirname = argv[1];

    DIR* dir = opendir(input_dirname);

    if (dir == NULL) {
        fprintf(stderr, "[DIR SIZE] Couldn't read dir (%s)\n", input_dirname);
        return 2;
    }

    int error = chdir(input_dirname);

    if (error == -1) {
        fprintf(stderr, "[DIR SIZE] Failed to enter dir (%s)\n", input_dirname);
        return 3;
    }

    struct dirent* current_file;
    long long total_size = 0;
    struct stat stats;

    while ((current_file = readdir(dir))) {
        stat(current_file->d_name, &stats);

        if (S_ISDIR(stats.st_mode))
            continue;

        printf("%s %ld\n", current_file->d_name, stats.st_size);
        total_size += stats.st_size; 
    }

    printf("TOTAL %lld\n", total_size);
    return 0;
}
