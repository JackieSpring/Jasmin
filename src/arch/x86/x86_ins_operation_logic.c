
#include "x86_ins_operation_logic.h"


typedef bool (* operation_overflow) (uint64_t, uint64_t, size_t, uint64_t * );

/*
 * INTERNAL AUXILIARY FUNCTIONS
*/

static bool turn_off_flag(uint64_t _, uint64_t __, size_t ___, uint64_t * _____) {
    return false;
}


/*
 * This function is a generic pattern for logical and mathematical calculations,
 * the jint, operands and nops parameters are inherited from the wrapper function
 * while carry_fun, aux_fun and ovf_fun are set from the wrapper to set or unset
 * CF AF and OF respectively.
 * carry_fun takes a special place because is the function that will calculate
 * the final result of the instruction (and set/unset CF depending on the return
 * value).
 * Setting aux_fun or ocf_fun to NULL will keep the respective flag unchanged.
 * This function updates PF, SF and ZF automatically depending on the result of 
 * the operation.
*/
static int x86_generic_operation
    ( jin_interpreter * jint, jin_operand * operands, size_t nops, operation_overflow carry_fun, operation_overflow aux_fun, operation_overflow ovf_fun ) 
{
    if ( nops != 2 || jint == NULL )
        return -1;

    jin_operand dst = operands[0];
    jin_operand src = operands[1];
    uint64_t op1buff = 0;
    uint64_t op2buff = 0;
    uint64_t ret = 0;
    uint64_t eflag = 0;
    bool carry;
   
    if ( dst.type == X86_OP_REG ){
        if( read_register( jint, dst.reg, &op1buff ) != dst.size )
            goto cleanup;
    }
    else if (dst.type == X86_OP_IMM )
        op1buff = dst.imm;
    
    else if ( dst.type == X86_OP_MEM ) {
        if( read_memory( jint, dst.mem, &op1buff, dst.size ) != dst.size )
            goto cleanup;
    }
    
    if ( src.type == X86_OP_REG ){
        if( read_register( jint, src.reg, &op2buff ) != src.size )
            goto cleanup;
    }
    else if (src.type == X86_OP_IMM )
        op2buff = src.imm;
    
    else if ( src.type == X86_OP_MEM ) {
        if( read_memory( jint, src.mem, &op2buff, src.size ) != src.size )
            goto cleanup;
    }
    
    carry = (carry_fun)(op1buff, op2buff, dst.size, &ret) ; // Calculate final result
    
// eflag update start
    read_register( jint, X86_REG_EFLAGS, &eflag );
    
    // update PF, SF, ZF
    x86_update_pf(ret, &eflag);
    x86_update_zf(ret, dst.size, &eflag);
    x86_update_sf(ret, dst.size, &eflag);
    
    // update CF
    if (carry == true)
        eflag |= X86_EFLAG_CF;
    else
        eflag &= ~X86_EFLAG_CF;
    
    // update AF
    if ( aux_fun != NULL ) {
        carry = (aux_fun)( op1buff << 1, op2buff << 1, 1, NULL );
        if ( carry == true)
            eflag |= X86_EFLAG_AF;
        else
            eflag &= ~X86_EFLAG_AF;
    }
    
    // update OF
    if ( ovf_fun != NULL ) {
        carry = (ovf_fun)( op1buff, op2buff, dst.size, NULL );
        if ( carry == true)
            eflag |= X86_EFLAG_OF;
        else
            eflag &= ~X86_EFLAG_OF;
    }

    write_register(jint, X86_REG_EFLAGS, &eflag);
// eflag update end

    if ( dst.type == X86_OP_REG )
        write_register(jint, dst.reg ,&ret);
    else if ( dst.type == X86_OP_MEM )
        write_memory( jint, dst.mem, &ret, dst.size );
    else 
        goto cleanup;
    
    return 0;

cleanup:
    return -1;
}


/*
 * INSTRUCTION HANDLERS
*/
// ADDITION AND SUBTRACTION
int x86_ins_add (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, unsigned_add_overflow, unsigned_add_overflow, signed_add_overflow );
}

int x86_ins_sub (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, unsigned_sub_overflow, unsigned_sub_overflow, signed_sub_overflow );
}

int x86_ins_inc (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand dst = operands[0];
    uint64_t ret = 0;
    uint64_t eflag = 0;
    bool carry;
    
    if ( dst.type == X86_OP_REG ){
        if( read_register( jint, dst.reg, &ret ) != dst.size )
            goto cleanup;
    }
    else if ( dst.type == X86_OP_MEM ) {
        if( read_memory( jint, dst.mem, &ret, dst.size ) != dst.size )
            goto cleanup;
    }
    
    ret = ret + 1;
    
    read_register( jint, X86_REG_EFLAGS, &eflag );
    
    // update PF, SF, ZF
    x86_update_pf(ret, &eflag);
    x86_update_zf(ret, dst.size, &eflag);
    x86_update_sf(ret, dst.size, &eflag);
    
    // update AF
    carry = unsigned_add_overflow( ret << 1, 1 << 1, 1, NULL );
    if ( carry == true)
        eflag |= X86_EFLAG_AF;
    else
        eflag &= ~X86_EFLAG_AF;
    
    // update OF
    carry = signed_add_overflow( ret, 1, dst.size, NULL );
    if ( carry == true)
        eflag |= X86_EFLAG_OF;
    else
        eflag &= ~X86_EFLAG_OF;

    write_register(jint, X86_REG_EFLAGS, &eflag);
    
    if ( dst.type == X86_OP_REG )
        write_register(jint, dst.reg ,&ret);
    else if ( dst.type == X86_OP_MEM )
        write_memory( jint, dst.mem, &ret, dst.size );
    else 
        goto cleanup;
    
    return 0;
    
cleanup:
    return -1;
}

int x86_ins_dec (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand dst = operands[0];
    uint64_t ret = 0;
    uint64_t eflag = 0;
    bool carry;
    
    if ( dst.type == X86_OP_REG ){
        if( read_register( jint, dst.reg, &ret ) != dst.size )
            goto cleanup;
    }
    else if ( dst.type == X86_OP_MEM ) {
        if( read_memory( jint, dst.mem, &ret, dst.size ) != dst.size )
            goto cleanup;
    }
    
    ret = ret - 1;
    
    read_register( jint, X86_REG_EFLAGS, &eflag );
    
    // update PF, SF, ZF
    x86_update_pf(ret, &eflag);
    x86_update_zf(ret, dst.size, &eflag);
    x86_update_sf(ret, dst.size, &eflag);
    
    // update AF
    carry = unsigned_sub_overflow( ret << 1, 1 << 1, 1, NULL );
    if ( carry == true)
        eflag |= X86_EFLAG_AF;
    else
        eflag &= ~X86_EFLAG_AF;
    
    // update OF
    carry = signed_sub_overflow( ret, 1, dst.size, NULL );
    if ( carry == true)
        eflag |= X86_EFLAG_OF;
    else
        eflag &= ~X86_EFLAG_OF;

    write_register(jint, X86_REG_EFLAGS, &eflag);
    
    if ( dst.type == X86_OP_REG )
        write_register(jint, dst.reg ,&ret);
    else if ( dst.type == X86_OP_MEM )
        write_memory( jint, dst.mem, &ret, dst.size );
    else 
        goto cleanup;
    
    return 0;
    
cleanup:
    return -1;
}

// LOGIC
    // AUXILIARY 
static bool inner_and (uint64_t op1, uint64_t op2, size_t size, uint64_t * total) {
    *total = op1 & op2;
    return false;   // set CF to 0
}
static bool inner_or (uint64_t op1, uint64_t op2, size_t size, uint64_t * total) {
    *total = op1 | op2;
    return false;
}
static bool inner_xor (uint64_t op1, uint64_t op2, size_t size, uint64_t * total) {
    *total = op1 ^ op2;
    return false;
}
  
int x86_ins_and (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, inner_and, NULL, turn_off_flag );
}
int x86_ins_or (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, inner_or, NULL, turn_off_flag );
}
int x86_ins_xor (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, inner_xor, NULL, turn_off_flag );
}

int x86_ins_not (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand dst = operands[0];
    uint64_t ret = 0;
    
    if ( dst.type == X86_OP_REG ){
        if( read_register( jint, dst.reg, &ret ) != dst.size )
            goto cleanup;
    }
    else if ( dst.type == X86_OP_MEM ) {
        if( read_memory( jint, dst.mem, &ret, dst.size ) != dst.size )
            goto cleanup;
    }
    
    ret = ~ret;
    
    if ( dst.type == X86_OP_REG )
        write_register(jint, dst.reg ,&ret);
    else if ( dst.type == X86_OP_MEM )
        write_memory( jint, dst.mem, &ret, dst.size );
    else 
        goto cleanup;
    
    return 0;
    
cleanup:
    return -1;
}



int x86_ins_lea(jin_interpreter * jint, jin_operand * operands, size_t nops ){
    if ( nops != 2 || jint == NULL )
        goto cleanup;
    
    jin_operand dst = operands[0];
    jin_operand src = operands[1];
    
    if( write_register( jint, dst.reg, &src.mem ) != src.size)
        goto cleanup;
    
    return 0;
cleanup:
    return -1;
}

