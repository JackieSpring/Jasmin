#ifndef _JASMIN_X86_GENERIC_H
#define _JASMIN_X86_GENERIC_H

#include <capstone/x86.h>

#include "interpreter.h"
#include "shared_macros.h"
#include "operation.h"
#include "jin_error.h"


#define X86_EFLAG_CF (1 << 0)
#define X86_EFLAG_PF (1 << 2)
#define X86_EFLAG_AF (1 << 4)
#define X86_EFLAG_ZF (1 << 6)
#define X86_EFLAG_SF (1 << 7)
#define X86_EFLAG_OF (1 << 11)


// For some reason, if a sub-register operand is filled with 1, the full 64bit operand gets filled with 1,
// to work around this i had to introduce more complex tests
static void x86_update_zf ( uint64_t val , register_size size, uint64_t * flag ) {
    char byte = 0;
    for ( ; size > 0; size--, val >>= 8 )
        if ( (uint8_t)val != 0 ) {
            *flag &= ~X86_EFLAG_ZF;
            return;
        }
    *flag |= X86_EFLAG_ZF;
}
static void x86_update_sf (uint64_t val, register_size size ,uint64_t * flag) {
    if ( ( val & ( 1 << ((size * 8) -1) ) ) != 0 )
        *flag |= X86_EFLAG_SF;
    else
        *flag &= ~X86_EFLAG_SF;
}

static void x86_update_pf ( uint64_t op1, uint64_t * flag ) {
    char res = 1;
    char i = 8;
    for ( ; i > 0; i--, op1 >>= 1 )
        res ^= (char)(op1 & 1 );
    
    if ( res )
        *flag |= X86_EFLAG_PF;
    else
        *flag &= ~X86_EFLAG_PF;

}

static jin_err x86_read_operand( jin_interpreter * jint, jin_operand op, uint64_t * buffer ) { // the type for the buffer is not optimal
    if ( op.type == X86_OP_REG ){
        if( read_register( jint, op.reg, buffer ) != op.size )
            return JIN_ERR_REG_CANNOT_READ;
    }
    else if (op.type == X86_OP_IMM )
        *buffer = op.imm;
    
    else if ( op.type == X86_OP_MEM ) {
        if( read_memory( jint, op.mem, buffer, op.size ) != op.size )
            return JIN_ERR_MEM_CANNOT_READ;
    }
    else
        return JIN_ERR_GENERIC;
    
    return JIN_ERR_OK;
}

static jin_err x86_write_operand( jin_interpreter * jint, jin_operand op, uint64_t * buffer ) {
    if ( op.type == X86_OP_REG ){
        if( write_register( jint, op.reg, buffer ) != op.size )
            return JIN_ERR_REG_CANNOT_READ;
    }
    else if ( op.type == X86_OP_MEM ) {
        if( write_memory( jint, op.mem, buffer, op.size ) != op.size )
            return JIN_ERR_MEM_CANNOT_READ;
    }
    else
        return JIN_ERR_GENERIC;
    
    return JIN_ERR_OK;
}

#endif