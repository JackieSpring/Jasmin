
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
 * Setting aux_fun or ovf_fun to NULL will keep the respective flag unchanged.
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
    jin_err jerr;
    
    jerr = x86_read_operand(jint, dst, &op1buff);
    if ( jerr != JIN_ERR_OK )
        return jerr;
    
    jerr = x86_read_operand(jint, src, &op2buff);
    if ( jerr != JIN_ERR_OK )
        return jerr;
    
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

    return (instruction_handler) x86_write_operand(jint, dst, &ret);
}


/*
 * INSTRUCTION HANDLERS
*/
// ADDITION AND SUBTRACTION
instruction_handler x86_ins_add (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, unsigned_add_overflow, unsigned_add_overflow, signed_add_overflow );
}

instruction_handler x86_ins_sub (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, unsigned_sub_overflow, unsigned_sub_overflow, signed_sub_overflow );
}

instruction_handler x86_ins_inc (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand dst = operands[0];
    uint64_t ret = 0;
    uint64_t eflag = 0;
    bool carry;
    jin_err jerr;
    
    jerr = x86_read_operand(jint, dst, &ret);
    if ( jerr != JIN_ERR_OK )
        return (instruction_handler) jerr;
    
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
    
    return (instruction_handler) x86_write_operand(jint, dst, &ret);
}

instruction_handler x86_ins_dec (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand dst = operands[0];
    uint64_t ret = 0;
    uint64_t eflag = 0;
    bool carry;
    jin_err jerr;
    
    jerr = x86_read_operand(jint, dst, &ret);
    if ( jerr != JIN_ERR_OK )
        return (instruction_handler) jerr;
    
    ret = ret - 1; 
    
// flag handle
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
// end flag write
    
    return (instruction_handler) x86_write_operand(jint, dst, &ret);

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

instruction_handler x86_ins_and (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, inner_and, NULL, turn_off_flag );
}
instruction_handler x86_ins_or (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, inner_or, NULL, turn_off_flag );
}
instruction_handler x86_ins_xor (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_operation( jint, operands, nops, inner_xor, NULL, turn_off_flag );
}

instruction_handler x86_ins_not (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand dst = operands[0];
    uint64_t ret = 0;
    jin_err jerr;
    
    jerr = x86_read_operand(jint, dst, &ret);
    if ( jerr != JIN_ERR_OK )
        return (instruction_handler) jerr;

    ret = ~ret;
    
    return (instruction_handler) x86_write_operand(jint, dst, &ret);
}

// LEA

instruction_handler x86_ins_lea(jin_interpreter * jint, jin_operand * operands, size_t nops ){
    if ( nops != 2 || jint == NULL )
        return JIN_ERR_GENERIC;
    
    jin_operand dst = operands[0];
    jin_operand src = operands[1];
    
    if( write_register( jint, dst.reg, &src.mem ) != src.size)
        return JIN_ERR_MEM_CANNOT_WRITE;;
    
    return JIN_ERR_OK;
}


instruction_handler x86_ins_shl(jin_interpreter * jint, jin_operand * operands, size_t nops ){};
instruction_handler x86_ins_sal(jin_interpreter * jint, jin_operand * operands, size_t nops ){};
instruction_handler x86_ins_shr(jin_interpreter * jint, jin_operand * operands, size_t nops ){};
instruction_handler x86_ins_sar(jin_interpreter * jint, jin_operand * operands, size_t nops ){};
