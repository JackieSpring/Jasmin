#ifndef _JASMIN_HASHTABLE_H
#define _JASMIN_HASHTABLE_H

#include "shared_macros.h"
#include "jin_string.h"

#define HTKEY_MAX_LEN 63

struct hashtable;
typedef struct hashtable hashtable;

typedef void * htvalue;
typedef unsigned char htkey [ HTKEY_MAX_LEN + 1 ];

typedef bool (* htiterator) ( htkey key, htvalue val, void * extra );

hashtable * create_hashtable();
void destroy_hashtable(hashtable * ht);

int add_to_hashtable(hashtable * ht, htkey key, htvalue entry);
int del_from_hashtable(hashtable * ht, htkey key);

int get_value_hashtable( hashtable * ht, htkey key, htvalue * buffer);

/*
 * The iterator function should return true to continue the iteration
 * or false to stop it
*/
int iter_hashtable ( hashtable *  ht, htiterator iterator, void * extra  );

size_t get_hashtable_size(hashtable * ht);      // return number of entries

#endif