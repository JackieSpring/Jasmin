#ifndef _JASMIN_MEMORY_H#define _JASMIN_MEMORY_H#include <stdio.h>#include <string.h>#include "shared_macros.h"#define MEM_32   32#define MEM_64   64#define MEM_DEFAULT 0xffffffffffffffff#define MEM_FAILURE 0xffffffffffffffff#define MEM_DEFAULT_OPTIONS NULL#define MEM_EXEC 0x1#define MEM_WRITE 0x2#define MEM_READ 0x4typedef unsigned int MEM_MODE;typedef uint64_t MEM_ADDR;typedef unsigned char MEM_PERMISSIONS;// For default size/address set to MEM_DEFAULTstruct options {    MEM_MODE    mode;        MEM_ADDR     stack_start;    MEM_ADDR     text_start;    MEM_ADDR     rodata_start;    MEM_ADDR     data_start;    MEM_ADDR     bss_start;        size_t     stack_size;    size_t     text_size;    size_t     rodata_size;    size_t     data_size;    size_t     bss_size;};int memory_init( struct options * opt );int memory_destroy();/* * @return {size_t} return the number of bytes written/readden  */size_t write_to_memory( MEM_ADDR address, void * buffer, size_t length );size_t read_from_memory(MEM_ADDR address, void * buffer, size_t length );/* This controllers have higher privileges than write_to_memory/read_from_memory, in fact * no read/write check is done; they do not affectd the behaviour of psuh/pop and are designed  * for memory management tasks rather then instructions execution * * !!!  In the *_stack functions the offset is calculated from the bottom of the stack, meanwhile * !!!  in all the others functions it's calculated from the start of the section. *  * @return  if success, return the virtual address where the data are written/readden, *          else returns MEM_FAILURE*/MEM_ADDR offset_read_text   ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_read_rodata ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_read_data   ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_read_bss    ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_read_stack  ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_write_text  ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_write_rodata( size_t offset, void * buffer, size_t length );MEM_ADDR offset_write_data  ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_write_bss   ( size_t offset, void * buffer, size_t length );MEM_ADDR offset_write_stack ( size_t offset, void * buffer, size_t length );/* * This functions mimcs a stack-like behaviour and are designed for  * fast memory insertions next to the last-used byte in the section * memory space, they are designed for easly execute instructions * like `dd <dword>' or push/pop directly to/from the stack * * @return  if success, return the virtual address where the data are written/readden, *          else returns MEM_FAILURE*/MEM_ADDR push_text  (void * buffer, size_t length);MEM_ADDR push_rodata(void * buffer, size_t length);MEM_ADDR push_data  (void * buffer, size_t length);MEM_ADDR push_bss   (void * buffer, size_t length);MEM_ADDR push_stack (void * buffer, size_t length);MEM_ADDR pop_stack  (void * buffer, size_t length);// FUNZIONI SPERIMENTALIvoid * get_real_memory_address( MEM_ADDR addr );#endif