
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
    jin_mode mode = jin_get_mode(jint);
    uint64_t data = 0;
    uint64_t rsp = 0;
    size_t err;
    
    if( read_register( jint, X86_REG_RSP, &rsp ) != QWORD ) return JIN_ERR_REG_CANNOT_READ;
    if( read_memory(jint, rsp, &data, src.size) != src.size) return JIN_ERR_MEM_CANNOT_READ;

    if      ( mode == JIN_MODE_32 ) rsp += DWORD;
    else if ( mode == JIN_MODE_64 ) rsp += QWORD;
    
    if( write_register( jint, X86_REG_RSP, &rsp ) != QWORD ) return JIN_ERR_REG_CANNOT_WRITE;
    if( write_register( jint, src.reg, &data ) != src.size ) return JIN_ERR_REG_CANNOT_WRITE;

    return JIN_ERR_OK;

}

instruction_handler x86_ins_push (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    jin_mode mode = jin_get_mode(jint);
    uint64_t buffer = 0;
    uint64_t data;
    jin_err jerr;
    
    jerr = x86_read_operand(jint, src, &data);
    if ( jerr != JIN_ERR_OK )
        return jerr;
    
    if( read_register( jint, X86_REG_RSP, &buffer ) != QWORD ) return JIN_ERR_REG_CANNOT_READ;

    if      ( mode == JIN_MODE_32 ) buffer -= DWORD;
    else if ( mode == JIN_MODE_64 ) buffer -= QWORD;
    
    if( write_register( jint, X86_REG_RSP, &buffer ) != QWORD ) return JIN_ERR_REG_CANNOT_WRITE;
    if( write_memory(jint, buffer, &data, src.size) != src.size ) return JIN_ERR_MEM_CANNOT_WRITE;

    return JIN_ERR_OK;

}
