#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

char read_buffer[BLOCK_SIZE+1] = "";
int _;

char* reverse(char* str) {
    size_t n = strlen(str);
    char tmp;

    for (int i = 0; i < n / 2; i++) {
        tmp = str[i];
        str[i] = str[n-i-1];
        str[n-i-1] = tmp;
    }

    return str;
}

long get_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
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
    if (argc != 3) {
        fprintf(stderr, "[REVERSE] Invalid arguments\n");
        return 1;
    }

    struct timespec timespec_buff_start, timespec_buff_end;
    clock_gettime(CLOCK_REALTIME, &timespec_buff_start);

    char* input_filename = argv[1];
    char* output_filename = argv[2];
    size_t end;

    FILE* input_file = fopen(input_filename, "r");
    FILE* output_file = fopen(output_filename, "w");

    long input_file_size = get_file_size(input_file);
    size_t block_count = input_file_size / ((size_t) BLOCK_SIZE );
    size_t remainder = input_file_size % ((size_t) BLOCK_SIZE );

    for (int i = 0; i <= block_count; i++) {
        fseek(input_file, -BLOCK_SIZE*i, SEEK_END);
        end = fread(read_buffer, sizeof(char), BLOCK_SIZE, input_file);
        read_buffer[end] = 0;
        fprintf(output_file, "%s", reverse(read_buffer));
    }

    fseek(input_file, 0, SEEK_SET);
    end = fread(read_buffer, sizeof(char), remainder, input_file);
    read_buffer[end] = 0;
    fprintf(output_file, "%s", reverse(read_buffer));

    fclose(input_file);
    fclose(output_file);

    clock_gettime(CLOCK_REALTIME, &timespec_buff_end);

    struct timespec diff = timespec_diff(timespec_buff_start, timespec_buff_end);

    printf("EXECUTION TIME: %lds %ldns\n", diff.tv_sec, diff.tv_nsec);

    return 0;
}