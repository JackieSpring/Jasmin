#ifndef _JASMIN_REGISTERS_H
#define _JASMIN_REGISTERS_H

#include <string.h>

#include "shared_macros.h"

typedef size_t register_id;
typedef uint16_t register_size;

struct register_list_struct;
typedef struct register_list_struct register_list;



register_list * create_register_list( register_id size );
void destroy_register_list( register_list * regl);

/*
 * add_register_to_list creates a new register in the given register list and
 * assign to it the given id.
 * If the register id is already in use the instruction will fail with -1.
 * add_register_partition_to_list will create a partition of an existing register
 * 'size` bytes long starting from the 'offset` byte of the register.
 * If the register doesn't exist or the partition id is already in use or the 
 * partition doesn't fit in the register, this function will fail with -1;
 * del_register_from_list destroy a register or partition and frees it's id, if
 * the target id is assigned to a register, all the partitions that points to it
 * will be freed. This function fails with -1 the id does not points to any
 * active register/partition.
*/
int add_register_to_list    ( register_list * regl, register_id regid, register_size size );
int add_register_partition_to_list( register_list * regl, register_id partid, register_id parentid, register_size offset, register_size size );
int del_register_from_list  ( register_list * regl, register_id regid );

/*
 * @regl    register list from where the register/partition will be written/readden
 * @regid   ID number of the register
 * @buffer  buffer where the register's data have to be stored or where to read the new data from,
 *          the size of the buffer must be at least equal to the size of the register
 *
 * @return  number of bytes readden / written
*/
register_size read_from_register  ( register_list * regl, register_id regid, void * buffer);
register_size write_into_register ( register_list * regl, register_id regid, void * buffer);

/*
 * Iterate through the register list and for each register encountered calls the
 * 'iter` function, this function can stop the iteration returning 'false`, or
 * 'true` to continue the loop.
 * The 'extra` pointer is an extra argument that can be passed to the iter
 * function during the loop.
*/
int iter_register_list ( register_list * regl, bool (* iter) ( register_id id, register_size, bool is_partition, void * extra ), void * extra );

#endif