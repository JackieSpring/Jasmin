#ifndef _JASMIN_SYMBOL_H
#define _JASMIN_SYMBOL_H

#include <string.h>

#include "shared_macros.h"
#include "hashtable.h"

typedef uint64_t symbol_value;
typedef unsigned char * symbol_key;

struct symbol_table_struct;
typedef struct symbol_table_struct symbol_table;


symbol_table * create_symbol_table();
int destroy_symbol_table(symbol_table * symt);


int add_symbol_to_table( symbol_table * symt, symbol_key sk, symbol_value val);
int del_symbol_from_table(symbol_table * symt, symbol_key key);

int get_symbol_value( symbol_table * symt, symbol_key sk, symbol_value * ret );

#endif