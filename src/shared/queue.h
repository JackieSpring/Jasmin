#ifndef _JASMIN_QUEUE_H
#define _JASMIN_QUEUE_H

#include "shared_macros.h"


struct queue_struct;
typedef struct queue_struct queue;

typedef void * quvalue;

queue * create_queue();
void destroy_queue(queue * qu);

int push_to_queue(queue * qu, quvalue val);
int pop_from_queue(queue * qu, quvalue * buffer);

bool queue_is_empty(queue * qu);
size_t get_queue_size(queue * qu);


#endif