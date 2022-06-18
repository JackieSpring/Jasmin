
#ifndef _JASMIN_STRING_H
#define _JASMIN_STRING_H

#include "shared_macros.h"
#include <string.h>
#include <ctype.h>

#define TRIMMED_CHARS "\t\n "

typedef unsigned char * string;

string str_lpbrk( const string str, const string chars );

string str_ltrim ( const string str );
string str_rtrim( string str );
string str_trim( string str );

bool str_isalphanum(const string str, size_t len);

#endif