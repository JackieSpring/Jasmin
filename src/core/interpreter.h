
#ifndef _JASMIN_INTERPRETER_H
#define _JASMIN_INTERPRETER_H

#include <assert.h>

#include <capstone/capstone.h>
#include <keystone/keystone.h>

#include "registers.h"
#include "isa.h"
#include "memory.h"
#include "symbol.h"
#include "shared_macros.h"
#include "jin_error.h"


typedef enum jin_arch {
    JIN_ARCH_ARM = 1,    // ARM architecture (including Thumb, Thumb-2)
    JIN_ARCH_ARM64,      // ARM-64, also called AArch64
    JIN_ARCH_MIPS,       // Mips architecture
    JIN_ARCH_X86,        // X86 architecture (including x86 & x86-64)
    JIN_ARCH_PPC,        // PowerPC architecture (currently unsupported)
    JIN_ARCH_SPARC,      // Sparc architecture
    JIN_ARCH_EVM,        // Ethereum Virtual Machine architecture
    JIN_ARCH_MAX,
} jin_arch;

typedef enum jin_mode {
    JIN_MODE_LITTLE_ENDIAN = 0,    // little-endian mode (default mode)
    JIN_MODE_BIG_ENDIAN = 1 << 30, // big-endian mode
    // arm / arm64
    JIN_MODE_ARM = 1 << 0,              // ARM mode
    JIN_MODE_THUMB = 1 << 4,       // THUMB mode (including Thumb-2)
    JIN_MODE_V8 = 1 << 6,          // ARMv8 A32 encodings for ARM
    // mips
    JIN_MODE_MICRO = 1 << 4,       // MicroMips mode
    JIN_MODE_MIPS3 = 1 << 5,       // Mips III ISA
    JIN_MODE_MIPS32R6 = 1 << 6,    // Mips32r6 ISA
    JIN_MODE_MIPS32 = 1 << 2,      // Mips32 ISA
    JIN_MODE_MIPS64 = 1 << 3,      // Mips64 ISA
    // x86 / x64
    JIN_MODE_16 = 1 << 1,          // 16-bit mode
    JIN_MODE_32 = 1 << 2,          // 32-bit mode
    JIN_MODE_64 = 1 << 3,          // 64-bit mode
    // ppc
    JIN_MODE_QPX = 1 << 4,         // Quad Processing eXtensions mode
    // sparc
    JIN_MODE_V9 = 1 << 4,          // SparcV9 mode
} jin_mode;


typedef struct jin_dynamic_memory {
    struct jin_dynamic_memory * next;
    uint8_t buffer[];
} jin_dynamic_memory;



struct jin_interpreter;
typedef struct jin_interpreter {
    register_list * regs;
    instruction_set * isa;
    memory_map * mem;
    symbol_table * symt;
    
    jin_arch arch;
    jin_mode mode;
    ks_engine * ks;
    csh cs;
    
    void * op_res;  // operand_resolver
    void * ip_reader; // instruction pointer 
    void * ip_writer;
    void * sp_reader;  // stack pointer
    void * sp_writer;
    
    jin_dynamic_memory * jin_dyn_mem_blocks_list;
    bool running;
} jin_interpreter;



// NEW OPERAND
typedef struct jin_operand {
    x86_op_type type;
    union {
		register_id reg;	  ///< register value for REG operand
		int64_t imm;		///< immediate value for IMM operand
		memory_addr mem;		///< base/index/scale/disp value for MEM operand
	};
	uint8_t size;

} jin_operand;

typedef struct jin_instruction {
    instruction_id id;
    size_t byte_size;
    size_t op_count;
    jin_operand * op;
} jin_instruction;




typedef jin_err (* instruction_handler) (jin_interpreter * jint, jin_operand * operands, size_t nops ) ;

typedef int (* operand_resolver) (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count );
    
    //special registers handlers
typedef register_size (* instruction_pointer_reader) (jin_interpreter * jint, void * buffer);
typedef register_size (* instruction_pointer_writer) (jin_interpreter * jint, void * buffer);

typedef register_size (* stack_pointer_reader) (jin_interpreter * jint, void * buffer);
typedef register_size (* stack_pointer_writer) (jin_interpreter * jint, void * buffer);

    // iterators
typedef bool (* register_iterator ) ( register_id id, register_size size, bool is_partition, void * extra );
typedef bool (* symbol_iterator )( const symbol_key symbol, symbol_value val, void * extra );



/*
 * How to initialize and set up the interpreter:
 *  1 - init_interpreter
 *  2 - create registers table and instruction table
 *  3 - add registers and instructions
 *  4 - set operand resolver (this step can even be done before)
 *  5 - set instruction and stack pointers access functions
*/

/*
 * INTERPRETER INITIALIZATION AND SETUP
*/
jin_interpreter * jin_init_interpreter( jin_arch arch, jin_mode mode );
void jin_fini_interpreter(jin_interpreter * ret);


    // architecture registers
int jin_create_register_table (jin_interpreter * jint, register_id size);
int jin_add_register (jin_interpreter * jint, register_id id, register_size size);
int jin_add_register_partition ( jin_interpreter * jint, register_id partid, register_id parentid, register_size offset, register_size size );
int jin_del_register (jin_interpreter * jint, register_id id );

/** int jin_create_register_table (jin_interpreter * jint, register_id size);
 * 
 * Creates the register table necessary for adding registers to the interpreter, no registers operation
 * can be done without it.
 * The 'size` arguments corresponds to the amount of registers/partitions that are going to be added later.
 * @jint        interpreter where to create the table
 * @size        number of entries in the table
 *
 * @return      0 if success, -1 if fail.
*/

/** int jin_add_register (jin_interpreter * jint, register_id id, register_size size);
 * 
 * Add a register to the interpreter and assign to it the given id, this function can fail 
 * if the size is 0 or the id is already in use.
 *
 * @jint        interpreter where to add the register
 * @id          id of the new register
 * @size        size of the new register (in bytes)
 *
 * @return      0 if success, -1 if fail
*/

/** int jin_add_register_partition ( jin_interpreter * jint, register_id partid, register_id parentid, register_size offset, register_size size );
 *
 * Creates a partition for a register and assign to it the given id, a partition
 * consume the same entry slot of a register inside the register table, when the
 * table is created, be sure to consider space both for registers and partitions.
 * Multiple partitions can be created on the same register, they can overlap, 
 * changes on a byte inside the partition affects the registr and all the partitions
 * that include that byte. The 'offset` parameter specifies the startting byte
 * of the register from where the partition will start.
 * This function can fail if the id is already in use, or if the id of the register
 * is not in use or is a partition itself, or if the partition doesn't fit the register.
 *
 * @jint        interpreter where to add the partition
 * @partid      id of the new partition
 * @parentid    id of the register
 * @offset      index of the first registr's byte of the partition
 * @size        size of the partition
 * 
 * @return      0 if success, -1 if fail
*/

/** int jin_del_register (jin_interpreter * jint, register_id id );
 * 
 * Deletes a register or a partition from the interpreter register table; if the 
 * id belongs to a partition, it will be deleted without affecting other partition
 * or the register, but if a register is deleted, all the partitions declared on
 * that registers will be deleted too. This function can fail if no register/partition
 * is assigned to the given id.
 *
 * @jint        interpreter from where delete the partition
 * @id          id of the register/partition to delete
 *
 * @return      o if success, -1 if fail
*/





    // architecture instructions
int jin_create_instruction_table(jin_interpreter * jint, instruction_id size);
int jin_add_instruction( jin_interpreter * jint, instruction_id id, instruction_handler func );
int jin_del_instruction( jin_interpreter * jint, instruction_id id );

    // operands resolver
int jin_set_operand_resolver( jin_interpreter * jint, operand_resolver func );

    // instruction and stack pointer access 
int jin_set_instruction_pointer_rw ( jin_interpreter * jint, instruction_pointer_reader rdf, instruction_pointer_writer wrf );
int jin_set_stack_pointer_rw ( jin_interpreter * jint, stack_pointer_reader rdf, stack_pointer_writer wrf );


/*
 * RESOURCES ACCESS
*/
    // registers
register_size read_register ( jin_interpreter * jint, register_id regid, void * buffer);
register_size write_register ( jin_interpreter * jint, register_id regid, void * buffer);

    // memory
size_t read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );
size_t write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );

    // memory high-privilege modification, do not use for architectures
size_t force_read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );
size_t force_write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );

/*
 * ARCHITECTURE-SPECIFIC FUNCTIONS ACCESS
*/
int resolve_operands (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count );
    
register_size read_instruction_pointer  (jin_interpreter * jint, void * buffer);
register_size write_instruction_pointer (jin_interpreter * jint, void * buffer);

register_size read_stack_pointer ( jin_interpreter *  jint, void * buffer );
register_size write_stack_pointer ( jin_interpreter *  jint, void * buffer );


/*
 * INTERPRETER UTILITY FUNCTIONS
*/
void * jin_malloc(jin_interpreter * jint, size_t size );
void jin_free_all(jin_interpreter * jint);

void jin_free_instruction( jin_instruction * ins );

jin_arch jin_get_arch(jin_interpreter * jint);
jin_mode jin_get_mode(jin_interpreter * jint);

void jin_start_interpreter (jin_interpreter * jint);
void jin_stop_interpreter ( jin_interpreter * jint );
bool jin_is_running ( jin_interpreter * jint );

    // string-form of a register id
const char * jin_get_reg_name( jin_interpreter * jint, register_id id );

    // extra argument used to pass data to/from out the loop
int jin_iterate_register(jin_interpreter * jint, register_iterator riter, void * extra );
int jin_iterate_symbol(jin_interpreter * jint, symbol_iterator siter, void * extra );





#endif