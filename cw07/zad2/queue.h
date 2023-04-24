#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

char queue_pop(char *queue);
void queue_push(char *queue, char byte);
bool queue_full(char *queue);
bool queue_empty(char *queue);

#endif