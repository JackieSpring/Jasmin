
#include "x86_ins_mov_push.h"

instruction_handler x86_ins_mov(jin_interpreter * jint, jin_operand * operands, size_t nops ){
    
    if ( nops != 2 || jint == NULL )
        goto cleanup;
    
    jin_operand dst = operands[0];
    jin_operand src = operands[1];
    uint64_t buffer;
    
    if ( src.type == X86_OP_REG ){
        if( read_register( jint, src.reg, &buffer ) != src.size )
            goto cleanup;
    }
    else if (src.type == X86_OP_IMM )
        buffer = src.imm;
    
    else if ( src.type == X86_OP_MEM ) {
        if( read_memory( jint, src.mem, &buffer, src.size ) != src.size )
            goto cleanup;
    }
    else {
        printf("UNKNOWN OPERAND\n");
        goto cleanup;
    }

    if ( dst.type == X86_OP_REG ){
        if( write_register( jint, dst.reg, &buffer ) != src.size)
            goto cleanup;
    }
    else if ( dst.type == X86_OP_MEM ){
        if( write_memory( jint, dst.mem, &buffer, src.size ) != src.size )
            goto cleanup;
    }
    else{
        printf("UNKNOWN OPERAND\n");
        goto cleanup;
    }
    
    return 0;

cleanup:
    return -1;
}

instruction_handler x86_ins_pop (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        goto cleanup;

    jin_operand src = operands[0];
    size_t err;
    uint64_t data = 0;
    uint64_t buffer = 0;
    
    err = read_register( jint, X86_REG_RSP, &buffer );
    if( err != QWORD )
        goto cleanup;

    err = read_memory(jint, buffer, &data, src.size);
    if( err != src.size)
        goto cleanup;
    
    if ( jin_get_mode(jint) == JIN_MODE_32 )
        buffer += 4;
    else if ( jin_get_mode(jint) == JIN_MODE_64 )
        buffer += 8;
    
    err = write_register( jint, X86_REG_RSP, &buffer );
    if( err != QWORD )
        goto cleanup;
    
    err = write_register( jint, src.reg, &data );
    if( err != src.size )
        goto cleanup;

    return 0;

cleanup:
    return -1;
}

instruction_handler x86_ins_push (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        goto cleanup;

    jin_operand src = operands[0];
    uint64_t data;
    uint64_t buffer = 0;
    
    if (src.type == X86_OP_REG)
        read_register(jint, src.reg, &data);
    else if ( src.type == X86_OP_IMM )
        data = src.imm;
    
    if( read_register( jint, X86_REG_RSP, &buffer ) != QWORD )
        goto cleanup;
    
    if ( jin_get_mode(jint) == JIN_MODE_32 )
        buffer -= 4;
    else if ( jin_get_mode(jint) == JIN_MODE_64 )
        buffer -= 8;
    
    if( write_register( jint, X86_REG_RSP, &buffer ) != QWORD )
        goto cleanup;

    if( write_memory(jint, buffer, &data, src.size) != src.size )
        goto cleanup;

    return 0;

cleanup:
    return -1;
}
