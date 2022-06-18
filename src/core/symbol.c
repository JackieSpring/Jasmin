
#include "symbol.h"

#define SYMS_ARRAY_SIZE 0xff


typedef struct symbol_entry_struct {
    symbol_key key;
    symbol_value val;
    struct symbol_entry * next;
} symbol_entry;


struct symbol_table_struct {
    hashtable * syms_array;
};


symbol_table * create_symbol_table() {
    symbol_table * ret = calloc(1, sizeof(symbol_table));
    if ( ret == NULL )
        goto cleanup;
    
    ret->syms_array = create_hashtable();
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
        destroy_hashtable(symt->syms_array);
        return 0;
    }
    return -1;
}


int add_symbol_to_table( symbol_table * symt, symbol_key sk, symbol_value val){
    if ( symt == NULL )
        return -1;
    return add_to_hashtable(symt->syms_array, sk, val);
}

int del_symbol_from_table(symbol_table * symt, symbol_key sk) {
    if ( symt == NULL )
        return -1;
    return del_from_hashtable(symt->syms_array, sk);
}

int get_symbol_value( symbol_table * symt, symbol_key sk, symbol_value * buffer ) {
    if ( symt == NULL )
        return -1;
    
    return get_value_hashtable( symt->syms_array , sk, buffer);
}



int iter_symbol_table( symbol_table * symt, bool (* iter)( const symbol_key symbol, symbol_value val, void * extra ), void * extra ) {
    return iter_hashtable( symt->syms_array, iter, extra );
}


