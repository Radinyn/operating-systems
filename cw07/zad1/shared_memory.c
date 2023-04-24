#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/shm.h>

#include "shared_memory.h"

#ifdef SYSTEM_V
static int get_shared_memory(const char *filename, int size) {
    key_t key = ftok(filename, 0);
    if (key == -1) {
        return -1;
    }
    return shmget(key, size, 0644 | IPC_CREAT);
}

char *attach_shared_memory(const char* filename, int size) {
    int shared_memory_id = get_shared_memory(filename, size);
    char *shared_memory;
    if (shared_memory_id == -1) {
        fprintf(stderr, "[ERROR] No identifier for file: %s\n", filename);
        return NULL;
    }
    shared_memory = shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (char*)(-1)) {
        fprintf(stderr, "[ERROR] Failed to load block with id %d\n", shared_memory_id);
        return NULL;
    }
    return shared_memory;
}

bool detach_shared_memory(char *shared_memory) {
    return (shmdt(shared_memory) != -1);
}

bool destroy_shared_memory(const char *filename) {
    int shared_memory_id = get_shared_memory(filename, 0);
    if(shared_memory_id == -1) {
        return false;
    }
    return (shmctl(shared_memory_id, IPC_RMID, NULL) != -1);
}
#endif



#ifdef POSIX
#include <errno.h>

static int get_shared_memory(const char* filename, int size) {
    int descriptor = shm_open(filename, O_CREAT | O_RDWR, 0644);
    if (descriptor == -1) {
        return -1;
    }
    if(ftruncate(descriptor, size) == -1) {
        perror("[ERROR] ftruncate() call");
        return -1;
    }
    return descriptor;
}

char *attach_shared_memory(const char* filename, int size) {
    int shared_memory_id = get_shared_memory(filename, size);
    if (shared_memory_id == -1) {
        fprintf(stderr, "[ERROR] Can't get file descriptor for: %s\n", filename);
        return NULL;
    }
    char *shared_memory = (char*) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);
    return shared_memory;
}

bool detach_shared_memory(char *shared_memory) {
    return (shmdt(shared_memory) != -1);
}

bool destroy_shared_memory(const char *filename) {
    return (shm_unlink(filename) != -1);
}
#endif