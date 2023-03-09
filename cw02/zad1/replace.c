#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

size_t _;

typedef union {
    int posix;
    FILE* standard;
} UniversalFile;

typedef enum {
    READ,
    WRITE,
} FILE_MODE;

UniversalFile universal_open(const char* filepath, FILE_MODE mode) {
    UniversalFile file;
#ifndef SYS
    file.standard = fopen(filepath, mode == READ ? "r" : "w");
#else
    file.posix = open(filepath, mode == READ ? O_RDONLY : O_WRONLY);
#endif
    return file;
}

size_t universal_size(UniversalFile file) {
#ifndef SYS
    fseek(file.standard, 0, SEEK_END);
    long size = ftell(file.standard);
    fseek(file.standard, 0, SEEK_SET);
    return size;
#else
    struct stat st;
    fstat(file.posix, &st);
    return st.st_size;
#endif
};

char* universal_read(UniversalFile file) {
    size_t size = universal_size(file);
    char* buff = (char*)calloc(size, sizeof(char));

#ifndef SYS
    _ = fread(buff, sizeof(char), size, file.standard);
#else
    _ = read(file.posix, buff, sizeof(char)*size);
#endif

    return buff;
}

void universal_write(UniversalFile file, size_t size, size_t count, void* src) {
#ifndef SYS
    _ = fwrite(src, size, count, file.standard);
#else
    _ = write(file.posix, src, size*count);
#endif
}

void universal_close(UniversalFile file) {
#ifndef SYS
    fclose(file.standard);
#else
    close(file.posix);
#endif
}

char* universal_full_read(const char* filepath) {
    UniversalFile file = universal_open(filepath, READ);
    char* buff = universal_read(file);
    universal_close(file);
    return buff;
}

void universal_full_write(const char* filepath, const char* buff) {
    UniversalFile file = universal_open(filepath, WRITE);
    universal_write(file, sizeof(char), strlen(buff), (void*)buff);
    universal_close(file);
}


int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "[REPLACE] Invalid arguments\n");
        return 1;
    }

    char from = argv[1][0];
    char into = argv[2][0];
    char* input_filename = argv[3];
    char* output_filename = argv[4];

    char* content = universal_full_read(input_filename);
    char* tmp = content;
    
    while (*tmp) {
        if (*tmp == from)
            *tmp = into;
        tmp++;
    }

    universal_full_write(output_filename, content);
    return 0;
}
