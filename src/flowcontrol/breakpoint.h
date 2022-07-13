
#ifndef _JASMIN_BREAKPOINT_H
#define _JASMIN_BREAKPOINT_H

#include "shared_macros.h"
#include "interpreter.h"
#include "jin_string.h"
#include "hashtable.h"
#include "numhashtable.h"


typedef memory_addr breakpoint_addr;
typedef string breakpoint_id;


int add_breakpoint( breakpoint_id id, breakpoint_addr bp );
int del_breakpoint( breakpoint_id id );

bool is_breakpoint( breakpoint_addr bp );

int iter_breakpoint( bool (* iter)( breakpoint_id id, breakpoint_addr ) );

#endif