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

LibWCMemory (*LibWCMemory_create)(size_t);
void (*LibWCMemory_init)(LibWCMemory*, size_t);
void (*LibWCMemory_clear)(LibWCMemory*);
void (*LibWCMemory_destruct)(LibWCMemory*);
char* (*LibWCMemory_get)(LibWCMemory*, size_t);
void (*LibWCMemory_pop)(LibWCMemory*, size_t);
void (*LibWCMemory_push)(LibWCMemory*, char* );

#endif
