#include "libwc.h"

char command[LIBWC_COMMAND_BUFF_SIZE] = "";

LibWCMemory LibWCMemory_create(size_t size) {
    return (LibWCMemory) {
        .arr = calloc(size, sizeof(char*)),
        .active = calloc(size, sizeof(bool)),
        .top = 0,
        .size = size,
    };
}

void LibWCMemory_init(LibWCMemory* LibWCMemory, size_t size) {
    LibWCMemory->arr = calloc(size, sizeof(char*));
    LibWCMemory->active = calloc(size, sizeof(bool));
    LibWCMemory->top = 0;
    LibWCMemory->size = size;
}

void LibWCMemory_clear(LibWCMemory* LibWCMemory) {
    for (size_t i = 0; i < LibWCMemory->top; i++)
        if (LibWCMemory->active[i])
            free(LibWCMemory->arr[i]);
    LibWCMemory->top = 0;
    memset(LibWCMemory->active, false, sizeof(bool)*(LibWCMemory->size));
}

void LibWCMemory_destruct(LibWCMemory* LibWCMemory) {
    LibWCMemory_clear(LibWCMemory);
    free(LibWCMemory->arr);
    free(LibWCMemory->active);
}

bool LibWCMemory_range_check(LibWCMemory* LibWCMemory, size_t index) {
    if (LibWCMemory->top <= index) {
        fprintf(stderr, "[LIB WC] INDEX OUT OF RANGE\n");
        return false; 
    }
    if (!LibWCMemory->active[index]) {
        fprintf(stderr, "[LIB WC] INDEX ALREADY REMOVED\n");
        return false;
    }
    return true;
}

char* LibWCMemory_get(LibWCMemory* LibWCMemory, size_t index) {
    if (LibWCMemory_range_check(LibWCMemory, index))
        return LibWCMemory->arr[index];
    return "";
}

void LibWCMemory_pop(LibWCMemory* LibWCMemory, size_t index) {
    if (LibWCMemory_range_check(LibWCMemory, index)) {
        free(LibWCMemory->arr[index]);
        LibWCMemory->active[index] = false;
    }
}

long get_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

char* get_file_content(char* filename) {
    FILE* file = fopen(filename, "r");
    size_t size = get_file_size(file);
    char* buff = calloc(size, sizeof(char));
    fread(buff, sizeof(char), size, file);
    fclose(file);
    return buff;
}

void LibWCMemory_push(LibWCMemory* LibWCMemory, char* input_filename) {
    char tmp_filename[] = "/tmp/wclib_XXXXXX";
    int tmp_file = mkstemp(tmp_filename);

    if (tmp_file == 0) {
        fprintf(stderr, "[LIB WC] FAILED TO CREATE A TEMPORARY FILE\n");
        return;
    }

    snprintf(command, LIBWC_COMMAND_BUFF_SIZE, "wc '%s' 1> '%s' 2>/dev/null", input_filename, tmp_filename);
    system(command);
    
    char* wc_output = get_file_content(tmp_filename);
    if (strlen(wc_output) == 0) {
        fprintf(stderr, "[LIB WC] FAILED TO READ THE INPUT FILE (%s)\n", input_filename);
        return;
    }

    snprintf(command, LIBWC_COMMAND_BUFF_SIZE, "rm -f '%s' 2>/dev/null", tmp_filename);
    system(command);

    if (LibWCMemory->top < LibWCMemory->size) {
        LibWCMemory->arr[LibWCMemory->top] = wc_output;
        LibWCMemory->active[LibWCMemory->top] = true;
        (LibWCMemory->top)++;
    } else {
        fprintf(stderr, "[LIB WC] NOT ENOUGH MEMORY\n");
    }
}
