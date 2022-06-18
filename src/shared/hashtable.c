
#include "hashtable.h"

#define HT_SIZE 0xff

typedef struct ht_entry_struct {
    htkey key;
    htvalue value;
    struct ht_entry_struct * next;
} ht_entry_struct;

struct hashtable {
    size_t arr_size;
    size_t nelem;
    ht_entry_struct ** array;
};



hashtable * create_hashtable() {
    hashtable * ret;
    
    ret = calloc(1, sizeof(hashtable));
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




static size_t hash_index( htkey key ) {
    size_t tab_index = 0;
    if ( key[0] != 0 )
        tab_index += key[0];
        if ( key[1] != 0 )
            tab_index += key[1];
    return (size_t)((unsigned char) tab_index);
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

static ht_entry_struct * create_entry( htkey key, htvalue val, ht_entry_struct * next ) {
    if ( key == NULL )
        return NULL;
    
    ht_entry_struct * ret;
    ret = calloc(1, sizeof(ht_entry_struct));
    if ( ret == NULL )
        goto cleanup;
    
    ret->value = val;
    ret->next = next;
    if( strncpy( ret->key, key, HTKEY_MAX_LEN ) != ret->key )
        goto cleanup;
    ret->key[HTKEY_MAX_LEN] = 0;
    
    if( strcpy( ret->key, key ) != ret->key )
        goto cleanup;
    
    
    return ret;

cleanup:
    if ( ret != NULL ) {
        free(ret);
    }
    return NULL;
}

static ht_entry_struct * find_entry(hashtable * ht, htkey key) {

    size_t tab_index;
    ht_entry_struct * ret;
    
    tab_index = hash_index(key);
    
    if ( tab_index >= ht->arr_size )
        goto cleanup;
    
    ret = ht->array[tab_index];
    
    while ( ret != NULL && (strcmp(key, ret->key) != 0) ) {
        ret = ret->next;
    }
    
    return ret;

cleanup:
    return NULL;
}


void destroy_hashtable(hashtable * ht) {
    ht_entry_struct * node;
    
    for ( size_t i = 0 ; i < ht->arr_size ; i++  ) {
        node = ht->array[i];
        destroy_entry_rec(node);
    }
    free(ht->array);
    free(ht);
}



int add_to_hashtable(hashtable * ht, htkey key, htvalue val) {
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

static int remove_entry_rec( ht_entry_struct * entry, htkey key ) {
    if ( entry == NULL )
        return -1;
    
    if ( entry->next == NULL )
        return -1;
    
    if ( strcmp(entry->next->key, key) != 0  )
        return remove_entry_rec(entry->next, key);
    
    ht_entry_struct * old = entry->next;
    entry->next = old->next;
    
    destroy_entry(old);
    return 0;
}

int del_from_hashtable(hashtable * ht, htkey key) {
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

int get_value_hashtable( hashtable * ht, htkey key, htvalue * buffer) {
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
int iter_hashtable ( hashtable *  ht, htiterator iterator, void * extra  ) {

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

size_t get_number_of_entries(hashtable * ht) {
    if ( ht == NULL )
        return 0;
    return ht->nelem;
}

