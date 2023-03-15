#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#define MAX_PATTERN_SIZE 255
#define RESOLVED_PATH_BUFFER_SIZE 4096
char resolved_path[RESOLVED_PATH_BUFFER_SIZE] = "";
char exe_path[RESOLVED_PATH_BUFFER_SIZE] = "";
char entry_path[RESOLVED_PATH_BUFFER_SIZE] = "";
char fread_buffer[MAX_PATTERN_SIZE + 1];
char next_depth_buffer[12] = "";

int main(int argc, char** argv) {
    int depth = -1;

    // CUSTOM DEPTH
    if (argc == 4) {
        depth = atoi(argv[3]);
        sprintf(next_depth_buffer, "%d", depth-1);
        if (depth < 1)
            return 0;
    } else if (argc != 3) {
        fprintf(stderr, "[REC] Invalid arguments\n");
        return 1;
    }

    if (strlen(argv[2]) > MAX_PATTERN_SIZE) {
        fprintf(stderr, "[REC] argv[1] too long\n");
        return 2;
    }

    size_t pattern_size = strlen(argv[2]);

    struct stat st;

    char* err;
    
    err = realpath(argv[0], exe_path);
    if (err == NULL && exe_path[0] != '/') {
        fprintf(stderr, "[REC] Error resolving path (%s)\n", argv[0]);
        return 3;
    }


    err = realpath(argv[1], resolved_path);
    if (err == NULL && resolved_path[0] != '/') {
        fprintf(stderr, "[REC] Error resolving path (%s)\n", argv[0]);
        return 3;
    }

    if (stat(resolved_path, &st) == -1) {
        fprintf(stderr, "[REC] Stat error (%s)\n", strerror(errno));
        return 4;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR* dir = opendir(resolved_path);

        if (dir == NULL) {
            fprintf(stderr, "[REC] Couldn't read dir (%s)\n", resolved_path);
            return 5;
        }

        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0) continue;
            if (strcmp(entry->d_name, "..") == 0) continue;

            strcpy(entry_path, resolved_path);
            strcat(entry_path, "/");
            strcat(entry_path, entry->d_name);

            if (fork() == 0) {
                if (argc == 4)
                    execl(exe_path, exe_path, entry_path, argv[2], next_depth_buffer, NULL);
                else
                    execl(exe_path, exe_path, entry_path, argv[2], NULL);
            }
                
        }
    } else {
        FILE* file = fopen(resolved_path, "r");
        size_t size = fread(fread_buffer, sizeof(char), pattern_size, file);
        fread_buffer[size] = 0;

        if (strcmp(fread_buffer, argv[2]) == 0) {
            printf("%s %d\n", resolved_path, getpid());
            fflush(NULL);
        }
    }

    // Wait for all the children, leave no orphans
    // Although not specified in the specification waiting is preferable 
    while (wait(NULL) > 0);

    return 0;
}
