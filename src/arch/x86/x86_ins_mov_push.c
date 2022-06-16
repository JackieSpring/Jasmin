
#include "x86_ins_mov_push.h"

instruction_handler x86_ins_mov(jin_interpreter * jint, jin_operand * operands, size_t nops ){
    
    if ( nops != 2 || jint == NULL )
        return JIN_ERR_GENERIC;
    
    jin_operand dst = operands[0];
    jin_operand src = operands[1];
    uint64_t buffer;
    jin_err jerr;
    
    jerr = x86_read_operand(jint, src, &buffer);
    if ( jerr != JIN_ERR_OK )
        return jerr;

    return x86_write_operand(jint, dst, &buffer);
}

instruction_handler x86_ins_pop (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    size_t err;
    uint64_t data = 0;
    uint64_t buffer = 0;
    
    err = read_register( jint, X86_REG_RSP, &buffer );
    if( err != QWORD )
        return JIN_ERR_REG_CANNOT_READ;

    err = read_memory(jint, buffer, &data, src.size);
    if( err != src.size)
        return JIN_ERR_MEM_CANNOT_READ;
    
    if ( jin_get_mode(jint) == JIN_MODE_32 )
        buffer += 4;
    else if ( jin_get_mode(jint) == JIN_MODE_64 )
        buffer += 8;
    
    err = write_register( jint, X86_REG_RSP, &buffer );
    if( err != QWORD )
        return JIN_ERR_REG_CANNOT_WRITE;
    
    err = write_register( jint, src.reg, &data );
    if( err != src.size )
        return JIN_ERR_REG_CANNOT_WRITE;

    return JIN_ERR_OK;

}

instruction_handler x86_ins_push (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t data;
    uint64_t buffer = 0;
    jin_err jerr;
    
    jerr = x86_read_operand(jint, src, &data);
    if ( jerr != JIN_ERR_OK )
        return jerr;
    
    if( read_register( jint, X86_REG_RSP, &buffer ) != QWORD )
        return JIN_ERR_REG_CANNOT_READ;
    
    if ( jin_get_mode(jint) == JIN_MODE_32 )
        buffer -= 4;
    else if ( jin_get_mode(jint) == JIN_MODE_64 )
        buffer -= 8;
    
    if( write_register( jint, X86_REG_RSP, &buffer ) != QWORD )
        return JIN_ERR_REG_CANNOT_WRITE;

    if( write_memory(jint, buffer, &data, src.size) != src.size )
        return JIN_ERR_MEM_CANNOT_WRITE;

    return JIN_ERR_OK;

}
