#ifndef _JASMIN_MEMORY_H
#define _JASMIN_MEMORY_H

#include <stdio.h>
#include <string.h>

#include "shared_macros.h"


#define MEM_32   32
#define MEM_64   64

#define MEM_DEFAULT 0xffffffffffffffff
#define MEM_FAILURE 0xffffffffffffffff
#define MEM_DEFAULT_OPTIONS NULL

#define MEM_EXEC 0x1
#define MEM_WRITE 0x2
#define MEM_READ 0x4

typedef unsigned int    memory_mode;
typedef uint64_t        memory_addr;
typedef unsigned char   memory_perm;

struct memory_map_struct;
typedef struct memory_map_struct memory_map;

// For default size/address set to MEM_DEFAULT
struct options {
    memory_mode    mode;
    
    memory_addr     stack_start;
    memory_addr     text_start;
    memory_addr     rodata_start;
    memory_addr     data_start;
    memory_addr     bss_start;
    
    size_t     stack_size;
    size_t     text_size;
    size_t     rodata_size;
    size_t     data_size;
    size_t     bss_size;
};


memory_map * create_memory_map( struct options * opt );
void destroy_memory_map(memory_map * mm);

/*
 * @return {size_t} return the number of bytes written/readden 
 */
size_t write_to_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length );
size_t read_from_memory( memory_map * mm, memory_addr address, void * buffer, size_t length );


/* This controllers have higher privileges than write_to_memory/read_from_memory, in fact
 * no read/write check is done; they do not affectd the behaviour of psuh/pop and are designed 
 * for memory management tasks rather then instructions execution.
*/
size_t force_write_to_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length );
size_t force_read_from_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length );


/* This controllers have higher privileges than write_to_memory/read_from_memory, in fact
 * no read/write check is done; they do not affectd the behaviour of psuh/pop and are designed 
 * for memory management tasks rather then instructions execution.
 * If buffer is set to NULL and length is set to 0, this funtion will return the start address
 * of the segment.
 *
 * !!!  In the *_stack functions the offset is calculated from the bottom of the stack, meanwhile
 * !!!  in all the others functions it's calculated from the start of the segment.
 * !!!  For NULL buffer the result address will be the bottom of the stack.
 * 
 * @return  if success, return the virtual address where the data are written/readden,
 *          else returns MEM_FAILURE
*/
memory_addr offset_read_text   ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_read_rodata ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_read_data   ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_read_bss    ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_read_stack  ( memory_map * mm,  size_t offset, void * buffer, size_t length );

memory_addr offset_write_text  ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_write_rodata( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_write_data  ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_write_bss   ( memory_map * mm,  size_t offset, void * buffer, size_t length );
memory_addr offset_write_stack ( memory_map * mm,  size_t offset, void * buffer, size_t length );

/*
 * This functions mimcs a stack-like behaviour and are designed for 
 * fast memory insertions next to the last-used byte in the section
 * memory space, they are designed for easly execute instructions
 * like `dd <dword>' or push/pop directly to/from the stack.
 * If buffer is set to NULL and length is set to 0, this functions
 * will return the address of the first unwritten byte (top of the stack)
 *
 * @return  if success, return the virtual address where the data are written/readden,
 *          else returns MEM_FAILURE
*/
memory_addr push_text  ( memory_map * mm, void * buffer, size_t length );
memory_addr push_rodata( memory_map * mm, void * buffer, size_t length );
memory_addr push_data  ( memory_map * mm, void * buffer, size_t length );
memory_addr push_bss   ( memory_map * mm, void * buffer, size_t length );
memory_addr push_stack ( memory_map * mm, void * buffer, size_t length );

memory_addr pop_stack  ( memory_map * mm, void * buffer, size_t length );


// FUNZIONI SPERIMENTALI
void * get_real_memory_pointer( memory_map * mm, memory_addr addr );

#endif