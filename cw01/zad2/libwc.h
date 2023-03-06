#ifndef __LIBWC_H__
#define __LIBWC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LIBWC_COMMAND_BUFF_SIZE 2048

typedef struct {
    char** arr;
    bool* active;
    size_t top;
    size_t size;
} LibWCMemory;

LibWCMemory LibWCMemory_create(size_t size);

void LibWCMemory_init(LibWCMemory* LibWCMemory, size_t size);

void LibWCMemory_clear(LibWCMemory* LibWCMemory);

void LibWCMemory_destruct(LibWCMemory* LibWCMemory);

char* LibWCMemory_get(LibWCMemory* LibWCMemory, size_t index);

void LibWCMemory_pop(LibWCMemory* LibWCMemory, size_t index);

void LibWCMemory_push(LibWCMemory* LibWCMemory, char* input_filename);

#endif
