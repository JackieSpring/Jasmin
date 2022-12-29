
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


#define MEM_SEG_TEXT    0
#define MEM_SEG_RODATA  1
#define MEM_SEG_DATA    2
#define MEM_SEG_BSS     3
#define MEM_SEG_STACK   4

#define MEM_STACK_DEFAULT_SIZE  MiB(8)
#define MEM_TEXT_DEFAULT_SIZE   KiB(8)
#define MEM_RODATA_DEFAULT_SIZE KiB(4)
#define MEM_DATA_DEFAULT_SIZE   KiB(4)
#define MEM_BSS_DEFAULT_SIZE    KiB(4) 

#define MEM_SEG_SYM_TEXT    "__text"
#define MEM_SEG_SYM_RODATA  "__rodata"
#define MEM_SEG_SYM_DATA    "__data"
#define MEM_SEG_SYM_BSS     "__bss"
#define MEM_SEG_SYM_STACK   "__stack"

#define MEM_TEXT_DEFAULT_ADDRESS_32     0x00400000
#define MEM_TEXT_DEFAULT_ADDRESS_64     0x08048000

#define MEM_STACK_DEFAULT_ADDRESS_32    0xbfa82000
#define MEM_STACK_DEFAULT_ADDRESS_64    0x7ffffffe9000

/** SEE memory.h */
#define MEM_EXEC 0x1
#define MEM_WRITE 0x2
#define MEM_READ 0x4



struct jin_interpreter;
typedef struct jin_interpreter jin_interpreter;




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

typedef enum jin_syntax {
	JIN_SYNTAX_INTEL =   1 << 0, // X86 Intel syntax - default on X86 (KS_OPT_SYNTAX).
	JIN_SYNTAX_ATT   =   1 << 1, // X86 ATT asm syntax (KS_OPT_SYNTAX).
	JIN_SYNTAX_NASM  =   1 << 2, // X86 Nasm syntax (KS_OPT_SYNTAX).
	JIN_SYNTAX_MASM  =   1 << 3, // X86 Masm syntax (KS_OPT_SYNTAX) - unsupported yet.
	JIN_SYNTAX_GAS   =   1 << 4, // X86 GNU GAS syntax (KS_OPT_SYNTAX).
} jin_syntax;

typedef struct jin_options {
    jin_arch    arch;
    jin_mode    mode;
    jin_syntax  syntax;
} jin_options;


typedef enum jin_state {
    JIN_STATE_RUNNING,              // Program is running, execute instructions
    JIN_STATE_PAUSE,                // Program is in pause, do not execute instructions
    JIN_STATE_INACTIVE,             // Program is not running, do not execute instructions
    JIN_STATE_TERMINATED,            // Terminate interpreter
} jin_state;


typedef enum jin_op_type {
	JIN_OP_INVALID = 0,  ///< uninitialized/invalid operand.
	JIN_OP_REG,          ///< Register operand.
	JIN_OP_IMM,          ///< Immediate operand.
	JIN_OP_MEM,          ///< Memory operand.
	JIN_OP_FP,           ///< Floating-Point operand.
} jin_op_type;

// NEW OPERAND
typedef struct jin_operand {
    jin_op_type type;
    union {
		register_id reg;      // < register id value
		int64_t imm;          // < immediate value
		memory_addr mem;      // < memory address
	};
	uint8_t size;

} jin_operand;

typedef struct jin_instruction {
    instruction_id id;
    size_t byte_size;
    size_t op_count;
    jin_operand * op;
    unsigned char ins_str[ 32 ];
    unsigned char op_str[160];
} jin_instruction;




typedef jin_err (* instruction_handler) (jin_interpreter * jint, jin_operand * operands, size_t nops ) ;
//typedef jin_err instruction_handler;

typedef int (* operand_resolver) (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count );
    
    //special registers handler
typedef register_size (* register_access_function)(jin_interpreter * jint, void * buffer);

    // iterators
typedef bool (* register_iterator ) ( register_id id, register_size size, bool is_partition, void * extra );
typedef bool (* symbol_iterator )( const symbol_key symbol, symbol_value val, void * extra );
typedef bool (* memory_iterator) ( segment_id id, memory_addr start, size_t size, bool is_stack, void * extra );



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
jin_interpreter * jin_init_interpreter( jin_options * jops );
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
 * @param param jint        interpreter where to create the table
 * @param param size        number of entries in the table
 *
 * @return      0 if success, -1 if fail.
*/

/** int jin_add_register (jin_interpreter * jint, register_id id, register_size size);
 * 
 * Add a register to the interpreter and assign to it the given id, this function can fail 
 * if the size is 0 or the id is already in use.
 *
 * @param jint        interpreter where to add the register
 * @param id          id of the new register
 * @param size        size of the new register (in bytes)
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
 * @param jint        interpreter where to add the partition
 * @param partid      id of the new partition
 * @param parentid    id of the register
 * @param offset      index of the first registr's byte of the partition
 * @param size        size of the partition
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
 * @param jint        interpreter from where delete the partition
 * @param id          id of the register/partition to delete
 *
 * @return      o if success, -1 if fail
*/





    // architecture instructions
int jin_create_instruction_table(jin_interpreter * jint, instruction_id size);
int jin_add_instruction( jin_interpreter * jint, instruction_id id, instruction_handler func );
int jin_del_instruction( jin_interpreter * jint, instruction_id id );

    // operands resolver
int jin_set_operand_resolver( jin_interpreter * jint, operand_resolver func );

/** int jin_set_operand_resolver( jin_interpreter * jint, operand_resolver func );
 * 
 * This function is used to set a traslator that converts the operands recived from
 * capstone to the jin_operand form. Because for each architecture capstone can 
 * use a different form of the operands, it's necessary that each architecture
 * teach to the interpreter how to achive this translation through this function.
 *
 *   int (* operand_resolver) (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count );
 * 
 * The 'raw_ins` of this callback is a pointer to the instruction structure
 * created by capstone after the disassembling, 'ret` and 'ret_op_count` are 
 * respectively pointers to where to store the final operands array and the
 * final numbers of operands in that array.
 *
 * @param jint        interpreter
 * @param func        pointer to the operand_resolver function
 *
 * @return      0 if the func was set succesfully, -1 if fail
*/




    // instruction and stack pointer access 
int jin_set_instruction_pointer_rw ( jin_interpreter * jint, register_access_function rdf, register_access_function wrf );
int jin_set_stack_pointer_rw ( jin_interpreter * jint, register_access_function rdf, register_access_function wrf );

/** int jin_set_instruction_pointer_rw ( jin_interpreter * jint, instruction_pointer_reader rdf, instruction_pointer_writer wrf );
 ** int jin_set_stack_pointer_rw ( jin_interpreter * jint, register_access_function rdf, register_access_function wrf );
 *
 * This two functions sets read/write handlers for the instruction and stack pointers,
 * because the ip and stackp registers can vary depending on the architecture and 
 * the interpreter mode, the given function are used internally to access those
 * registers.
 *
 *   register_size (* register_access_function)(jin_interpreter * jint, void * buffer);
 *
 * The buffer points to a memory area where the function has to read the data to write
 * into the registers or write the buffer the result, the return value of this handlers
 * must be the number of bytes readden/written from/to the buffer.
 *
 * @param jint        interpreter
 * @param rdf         read function used to read from the instruction/stack pointer
 * @param wrf         write function used to write to the instruction pointer
 *
 * @return      0 if success, -1 if fail 
*/



/* ############################################################################
 *                              RESOURCES ACCESS
 * ############################################################################
*/
    // symbols
int jin_add_symbol( jin_interpreter * jint , symbol_key sk, symbol_value val);
int jin_del_symbol( jin_interpreter * jint , symbol_key sk);
int jin_get_symbol( jin_interpreter * jint, symbol_key sk, symbol_value * ret );

    // registers
register_size read_register ( jin_interpreter * jint, register_id regid, void * buffer);
register_size write_register ( jin_interpreter * jint, register_id regid, void * buffer);

/** register_size read_register ( jin_interpreter * jint, register_id regid, void * buffer);
 ** register_size write_register ( jin_interpreter * jint, register_id regid, void * buffer);
 *
 * Read / write from/to the register/partition assigned to the id, the 'buffer` argument
 * points to the memory location where to write the data recived by the register or
 * where to recover the data that has to be written in the register. The number of bytes
 * readden/written depends on the size of the register, so buffer must be at least
 * the same size of the register.
 *
 * @param jint        interpreter
 * @param regid       id of the register where to read/write
 * @param buffer      buffer where the register data will be stored / containing the data
 *              that has to be written in the register
 *
 * @return      number of bytes readden/written, 0 if the register is not in use
*/






    // memory
size_t read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );
size_t write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );

/** size_t read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length )
 ** size_t write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length )
 *
 * Read / write 'length` bytes from/to memory at the given address, the 'buffer`
 * argument  points to the memory location where to write the data recived by the 
 * memory or where to recover the data that has to be written in the memory.
 * The number of bytes readden/written depends on the length argument, so buffer 
 * must be at least 'length` bytes long. If the memory address is not readable/writable
 * or if 'buffer` is NULL, or the size does not fit the memory segment this function
 * will fail.
 * The write operation can increase the pointer to the last written byte of the 
 * segment used internally by the memory for stack-like operations.
 *
 * @param jint          interpreter
 * @param addr          memory address where to read/write
 * @param buffer        buffer where the data will be stored / containig the data that has
 *                      to be written in memory.
 * @param length        number of bytes to read/write
 *
 * @return              number of bytes readden/written. 0 if fail.
*/



    // memory high-privilege modification, do not use for architectures
size_t force_read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );
size_t force_write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length );

/** size_t force_read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length )
 ** size_t force_write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length )
 *
 * This functions are almost identical to read_memory/write_memory, the only difference
 * is that they do not check for read/write permissions, so any read/write operation is
 * as long as the address points to a valid memory region and the length fits the boundary
 * of the segment and the top-byte pointer is not modified by the write function.
 * Because of the special privileges they have, these functions must not be used inside
 * architecture instructions, their use should be reserved to commands or memory management
 * operations.
 *
*/


/* ############################################################################
 *                      ARCHITECTURE-SPECIFIC FUNCTIONS ACCESS
 * ############################################################################
*/
int resolve_operands (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count );
    
register_size read_instruction_pointer  (jin_interpreter * jint, void * buffer);
register_size write_instruction_pointer (jin_interpreter * jint, void * buffer);

register_size read_stack_pointer ( jin_interpreter *  jint, void * buffer );
register_size write_stack_pointer ( jin_interpreter *  jint, void * buffer );


/* ############################################################################
 *                      INTERPRETER UTILITY FUNCTIONS
 * ############################################################################
*/
jin_arch jin_get_arch(jin_interpreter * jint);
jin_mode jin_get_mode(jin_interpreter * jint);
jin_syntax jin_get_syntax(jin_interpreter * jint);

int jin_set_state (jin_interpreter * jint, jin_state state);
jin_state jin_get_state(jin_interpreter * jint);

void jin_free_instruction( jin_instruction * ins );

/** void jin_free_instruction( jin_instruction * ins )
 *
 * Frees an instruction. Eevry jin_instruction must be freed after it's use.
 *
 * @param ins         The instruction to free.
*/


const char * jin_get_reg_name( jin_interpreter * jint, register_id id );

/** const char * jin_get_reg_name( jin_interpreter * jint, register_id id )
 *
 * Translate a register id into the mnemonic of that register, for example if the 
 * x86 architecture is in use:
 *              jin_get_reg_name( jint, X86_REG_RAX ) -> "rax"
 * the translation depends on the architecture of the interpreter, so the same id
 * can be translated in a different register between different architectures, also
 * it strictly depends on capstone library, so if the id used for identifying the
 * register is different from the capstone code fore that register, the traslation
 * may result wrong.
 *
 * @param jint        interpreter
 * @param id          id of the register
 *
 * @return      a string representing the register mnemonic
*/



int jin_iterate_register(jin_interpreter * jint, register_iterator riter, void * extra );
int jin_iterate_symbol(jin_interpreter * jint, symbol_iterator siter, void * extra );
int jin_iterate_memory ( jin_interpreter * jint, memory_iterator miter, void * extra);

/** int jin_iterate_register(jin_interpreter * jint, register_iterator riter, void * extra )
 ** int jin_iterate_symbol(jin_interpreter * jint, symbol_iterator siter, void * extra )
 *
 * This two functions iterate respectively through the registers table or the symbols
 * table. The *_iterator arguments are functions called by the iterator for each
 * element found, the arguments passed to them depends on the table that is being 
 * iterated but they shares similars definitions:
 *      bool (* `table iterator` ) ( `table arguments` , void * extra )
 * the return type of the function is used to control the iteration process, if
 * `true` is returned then the iteration must continue, otherwise if `false` is 
 * returned the iteration must stop.
 * The `extra` pointer is used for passing arguments from outside the loop to
 * the *_iterator function.
 *
 * @param jint        interpreter
 * @param riter/siter The iterator function, this parameter can't be NULL
 * @param extra       a pointer that is passed to the iterator function at every iteration.
 *
 * @return      0 if success, -1 if fail
*/



jin_err jin_assemble( jin_interpreter * jint, unsigned char * asm_code, uint64_t ins_p, uint8_t ** bytecode, size_t * bytecode_size, size_t * ins_count );

jin_err jin_disassemble( jin_interpreter * jint, uint64_t ins_p, uint8_t * bytecode, size_t size, jin_instruction ** ret_insn_p );

jin_err jin_execute( jin_interpreter * jint, jin_instruction * ins );

/** 
 * privileged memory access
 * IN offset_*, if buffer=NULL and length=0 @return start address of segment
 * IN push/pop, if buffer=NULL and length=0 @return last segment's last address
 */
memory_addr offset_write_segment ( jin_interpreter * jint , segment_id id, size_t offset, void * buffer, size_t length );
memory_addr offset_read_segment ( jin_interpreter * jint, segment_id id, size_t offset, void * buffer, size_t length );
memory_addr push_segment ( jin_interpreter * jint, segment_id id, void * buffer, size_t length );
memory_addr pop_segment ( jin_interpreter * jint, segment_id id, void * buffer, size_t length );

// Check memory permissions of given address
bool check_perm_memory( jin_interpreter * jint, memory_addr addr, memory_perm perm );

// get real memory pointer from virtual memory pointer
void * get_effective_pointer( jin_interpreter * jint, memory_addr address );

// set segment where new instructions will be pushed (dafault .text)
int jin_set_working_segment( jin_interpreter * jint , segment_id id );
segment_id jin_get_working_segment( jin_interpreter * jint );

// set code entrypoint address -> IP initial value
int jin_set_entrypoint( jin_interpreter * jint, memory_addr addr );
memory_addr jin_get_entrypoint( jin_interpreter * jint );

// set stack start address -> SP initial value
int jin_set_stackbase( jin_interpreter * jint, memory_addr addr );
memory_addr jin_get_stackbase( jin_interpreter * jint ); 


string jin_state_to_string( jin_state state );
string jin_arch_to_string( jin_arch arch );
string jin_mode_to_string(jin_arch arch, jin_mode mode );
string jin_endianess_to_string(jin_mode mode);


#endif