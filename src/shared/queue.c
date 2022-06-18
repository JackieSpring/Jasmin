#include "queue.h"


typedef struct queue_entry_struct {
    quvalue * val;
    struct queue_entry_struct * next;
    struct queue_entry_struct * prev;
} queue_entry;


struct queue_struct {
    size_t  nentries;
    struct queue_entry_struct * head;
    struct queue_entry_struct * tail;
};


queue * create_queue() {
    queue * ret;
    
    ret = calloc(1, sizeof(queue));
    if ( ret == NULL )
        return NULL;
    
    ret->nentries = 0;
    ret->head = NULL;
    ret->tail = NULL;
    
    return ret;
}

static void destroy_entry ( queue_entry * entry ) {
    if ( entry == NULL )
        return;
    
    free(entry);
}

void destroy_queue(queue * qu) {
    if ( qu == NULL )
        return;
    
    queue_entry * node = qu->head;
    queue_entry * old = qu->head;
    
    while( node != NULL ) {
        old = node;
        node = old->next;
        destroy_entry(old);
    }
    
    free(qu);
    return;
}

static queue_entry * create_entry( quvalue val, queue_entry * next, queue_entry * prev ) {
    queue_entry * ret = calloc(1, sizeof(queue_entry));
    if(ret == NULL)
        return NULL;
    
    ret->val = val;
    ret->next = next;
    ret->prev = prev;
    
    return ret;
}

int push_to_queue(queue * qu, quvalue val) {
    if ( qu == NULL )
        return -1;
    
    queue_entry * old = qu->head;
    queue_entry * new_entry = create_entry(val, old, NULL );
    if ( new_entry == NULL )
        return -1;
    qu->head = new_entry;
    if ( old != NULL )
        old->prev = new_entry;
    else
        qu->tail = new_entry;
        
    qu->nentries++;
    
    return 0;
    
}

int pop_from_queue(queue * qu, quvalue * buffer) {
    if( qu == NULL )
        return -1;
    
    queue_entry * old = qu->tail;
    if ( old == NULL ){
        return -1;
    }
    
    if (buffer != NULL )
        *buffer = old->val;
    
    qu->tail = old->prev;
    if ( old->prev == NULL )
        qu->head = NULL;
    else {
        old->prev->next = NULL;
    }
    qu->nentries--;
    destroy_entry(old);
    return 0;
    
}

bool queue_is_empty(queue * qu) {
    if ( qu == NULL || qu->nentries == 0 )
        return true;
    
    return false;
}

size_t get_queue_size(queue * qu) {
    if ( qu == NULL )
        return 0;
    
    return qu->nentries;
}

