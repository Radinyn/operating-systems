#ifndef __DLLMANAGER_H__
#define __DLLMANAGER_H__

#ifdef USE_DLL
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#define WCLIB_SYMBOL_TABLE_SIZE 7
void* _dll_ptrs[WCLIB_SYMBOL_TABLE_SIZE];

char* _dll_symbol_names[] = {
    "LibWCMemory",
    "LibWCMemory_init",
    "LibWCMemory_clear",
    "LibWCMemory_destruct",
    "LibWCMemory_get",
    "LibWCMemory_pop",
    "LibWCMemory_push",
};

void load_dll_symbols(const char* filepath) {
    void *handle = dlopen(filepath, RTLD_LAZY);

    if (handle == NULL) {
        fprintf(stderr, "[DLL MANAGER] DLL NOT FOUND (%s)\n", filepath);
        return;
    }

    // rapidly dwindling sanity of C developers
    *(void **) (&LibWCMemory_create) = dlsym(handle,"LibWCMemory_create");
    *(void **) (&LibWCMemory_init) = dlsym(handle,"LibWCMemory_init");
    *(void **) (&LibWCMemory_clear) = dlsym(handle,"LibWCMemory_clear");
    *(void **) (&LibWCMemory_destruct) = dlsym(handle,"LibWCMemory_destruct");
    *(void **) (&LibWCMemory_get) = dlsym(handle,"LibWCMemory_get");
    *(void **) (&LibWCMemory_pop) = dlsym(handle,"LibWCMemory_pop");
    *(void **) (&LibWCMemory_push) = dlsym(handle,"LibWCMemory_push");
}

#else
void load_dll_symbols(const char* filepath) {}
#endif

#endif


