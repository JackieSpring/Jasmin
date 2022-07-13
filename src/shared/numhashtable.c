
#include "numhashtable.h"

#define HT_SIZE 0xff

typedef struct ht_entry_struct {
    nhtkey key;
    nhtvalue value;
    struct ht_entry_struct * next;
} ht_entry_struct;

struct numhashtable {
    size_t arr_size;
    size_t nelem;
    ht_entry_struct ** array;
};



numhashtable * create_numhashtable() {
    numhashtable * ret;
    
    ret = calloc(1, sizeof(numhashtable));
    if ( ret == NULL )  
        goto cleanup;
    
    ret->arr_size = HT_SIZE;
    ret->nelem = 0;
    ret->array= calloc( ret->arr_size, sizeof( ht_entry_struct ) );
    if ( ret->array == NULL )
        goto cleanup;

    return ret;    

cleanup:
    if ( ret != NULL )
        free(ret);
    
    return NULL;
}




static size_t hash_index( nhtkey key ) {
    return key % HT_SIZE;
}

static void destroy_entry( ht_entry_struct * htentry ) {
    if ( htentry == NULL )
        return;
    free(htentry);
}

static void destroy_entry_rec ( ht_entry_struct * htentry ){
    if ( htentry == NULL )
        return;
    ht_entry_struct * next;
    next = htentry->next;
    
    destroy_entry( htentry );
    destroy_entry_rec(next);
}

static ht_entry_struct * create_entry( nhtkey key, nhtvalue val, ht_entry_struct * next ) {
    if ( key == NULL )
        return NULL;
    
    ht_entry_struct * ret;
    ret = calloc(1, sizeof(ht_entry_struct));
    if ( ret == NULL )
        goto cleanup;
    
    ret->value = val;
    ret->next = next;
    ret->key = key;
    
    return ret;

cleanup:
    if ( ret != NULL ) {
        free(ret);
    }
    return NULL;
}

static ht_entry_struct * find_entry(numhashtable * ht, nhtkey key) {

    size_t tab_index;
    ht_entry_struct * ret;
    
    tab_index = hash_index(key);
    
    if ( tab_index >= ht->arr_size )
        goto cleanup;
    
    ret = ht->array[tab_index];
    
    while ( ret != NULL && key != ret->key ) {
        ret = ret->next;
    }
    
    return ret;

cleanup:
    return NULL;
}


void destroy_numhashtable(numhashtable * ht) {
    ht_entry_struct * node;
    
    for ( size_t i = 0 ; i < ht->arr_size ; i++  ) {
        node = ht->array[i];
        destroy_entry_rec(node);
    }
    free(ht->array);
    free(ht);
}



int add_to_numhashtable(numhashtable * ht, nhtkey key, nhtvalue val) {
    if ( ht == NULL || key == NULL )
        return -1;
    
    size_t index = hash_index(key);
    ht_entry_struct * new_entry = create_entry(key, val, ht->array[index]);
    if ( new_entry == NULL )
        return -1;
    
    ht->array[index] = new_entry;
    ht->nelem++;
    
    return 0;
    
}

static int remove_entry_rec( ht_entry_struct * entry, nhtkey key ) {
    if ( entry == NULL )
        return -1;
    
    if ( entry->next == NULL )
        return -1;
    
    if ( entry->next->key != key )
        return remove_entry_rec(entry->next, key);
    
    ht_entry_struct * old = entry->next;
    entry->next = old->next;
    
    destroy_entry(old);
    return 0;
}

int del_from_numhashtable(numhashtable * ht, nhtkey key) {
    if ( ht == NULL || key == NULL )
        return -1;

    size_t index = hash_index(key);
    ht_entry_struct fake_entry = {
        .next = ht->array[index],
    };
    
    if ( remove_entry_rec(&fake_entry, key) < 0)
        return -1;
    
    ht->array[index] = fake_entry.next;
    ht->nelem--;
    return 0;
}

int get_value_numhashtable( numhashtable * ht, nhtkey key, nhtvalue * buffer) {
    if ( ht == NULL )
        return -1;
    
    ht_entry_struct * node = find_entry(ht, key);
    
    if (node == NULL)
        return -1;
    if(buffer != NULL )
        *buffer = node->value;
    return 0;
}

/*
 * The iterator function should return true to continue the iteration
 * or false to stop it
*/
int iter_numhashtable ( numhashtable *  ht, nhtiterator iterator, void * extra  ) {

    if ( ht == NULL || iterator == NULL )
        return -1;

    ht_entry_struct * node;
    
    for ( size_t i = 0 ; i < ht->arr_size ; i++  ) {
        node = ht->array[i];
        
        for ( ; node != NULL ; node = node->next ) {
            if ( (iterator)( node->key, node->value, extra ) == false )
                return 0;
        }
    }
    
    return 0;
        
}

size_t get_numhashtable_size(numhashtable * ht) {
    if ( ht == NULL )
        return 0;
    return ht->nelem;
}

