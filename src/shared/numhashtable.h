#ifndef _JASMIN_NUMHASHTABLE_H
#define _JASMIN_NUMHASHTABLE_H

#include "shared_macros.h"
#include "jin_string.h"

#define HTKEY_MAX_LEN 63

struct numhashtable;
typedef struct numhashtable numhashtable;

typedef void * nhtvalue;
typedef uint64_t nhtkey ;

typedef bool (* nhtiterator) ( nhtkey key, nhtvalue val, void * extra );

numhashtable * create_numhashtable();
void destroy_numhashtable(numhashtable * ht);

int add_to_numhashtable(numhashtable * ht, nhtkey key, nhtvalue entry);
int del_from_numhashtable(numhashtable * ht, nhtkey key);

int get_value_numhashtable( numhashtable * ht, nhtkey key, nhtvalue * buffer);

/*
 * The iterator function should return true to continue the iteration
 * or false to stop it
*/
int iter_numhashtable ( numhashtable *  ht, nhtiterator iterator, void * extra  );

size_t get_numhashtable_size(numhashtable * ht);      // return number of entries

#endif