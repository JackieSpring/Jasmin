
#include "symbol.h"

#define SYMS_ARRAY_SIZE 0xff


typedef struct symbol_entry_struct {
    symbol_key key;
    symbol_value val;
    struct symbol_entry * next;
} symbol_entry;


struct symbol_table_struct {
    size_t nsyms;
    size_t syms_arr_size;
    symbol_entry ** syms_array;
};


symbol_table * create_symbol_table() {
    symbol_table * ret = calloc(1, sizeof(symbol_table));
    if ( ret == NULL )
        goto cleanup;
    
    ret->nsyms = 0;
    ret->syms_arr_size = SYMS_ARRAY_SIZE;
    ret->syms_array = calloc( SYMS_ARRAY_SIZE, sizeof(symbol_entry) );
    if ( ret->syms_array == NULL )
        goto cleanup;
    
    return ret;    
    
cleanup:
    if ( ret != NULL )
        free(ret);
    return NULL;
}

int destroy_symbol_table(symbol_table * symt) {
    if ( symt != NULL ) {
        free(symt->syms_array);
        free(symt);
        return 0;
    }
    
    return -1;
}


static symbol_entry * find_symbol_entry(symbol_table * symt, symbol_key key) {

    unsigned char tab_index = 0;
    symbol_entry * ret;
    
    if ( key[0] != 0 )
        tab_index += key[0];
        if ( key[1] != 0 )
            tab_index += key[1];
    
    if ( tab_index >= symt->syms_arr_size )
        goto cleanup;
    
    ret = symt->syms_array[tab_index];
    
    while ( ret != NULL && (strcmp(key, ret->key) != 0) ) {
        ret = ret->next;
    }
    
    return ret;

cleanup:
    return NULL;
}


int add_symbol_to_table( symbol_table * symt, symbol_key sk, symbol_value val){

    unsigned char tab_index = 0;
    symbol_entry * ret;

    if ( symt == NULL || sk == NULL )
        goto cleanup;

    if ( find_symbol_entry(symt, sk) != NULL )
        goto cleanup;
    
    if ( sk[0] != 0 )
        tab_index += sk[0];
        if ( sk[1] != 0 )
            tab_index += sk[1];

    if ( tab_index >= symt->syms_arr_size )
        goto cleanup;
    
    ret = calloc( 1, sizeof(symbol_entry) );
    if ( ret == NULL )
        goto cleanup;
    
    ret->val = val;
    ret->next = symt->syms_array[tab_index];
    ret->key = malloc(strlen(sk));
    if( ret->key == NULL )
        goto cleanup;
    
    if( strcpy( ret->key, sk ) != ret->key )
        goto cleanup;
    
    symt->syms_array[tab_index] = ret;
    symt->nsyms++;
    
    return 0;

cleanup:
    if (ret != NULL) {
        if ( ret->key )
            free(ret->key);
        free(ret);
    }
    
    return -1;
}

int del_symbol_from_table(symbol_table * symt, symbol_key sk) {
    unsigned char tab_index = 0;
    symbol_entry * old;
    
    if ( symt == NULL || sk == NULL )
        goto cleanup;

    if ( find_symbol_entry(symt, sk) == NULL )
        goto cleanup;
    
    if ( sk[0] != 0 )
        tab_index += sk[0];
        if ( sk[1] != 0 )
            tab_index += sk[1];
    
    if ( tab_index >= symt->syms_arr_size )
        goto cleanup;
    
    old = symt->syms_array[tab_index];
    
    free( old->key );
    symt->syms_array[tab_index] = old->next;
    symt->nsyms--;
    free(old);
    
    return 0;
    
cleanup:
    return -1;
}

int get_symbol_value( symbol_table * symt, symbol_key sk, symbol_value * buffer ) {
    symbol_entry * ret;

    if ( symt == NULL || sk == NULL )
        return -1;

    ret = find_symbol_entry(symt, sk);
    if (ret == NULL)
        return -1;

    *buffer = ret->val;
    return 0;
}
