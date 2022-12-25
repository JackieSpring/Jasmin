
#include "x86_ins_jmp_call.h"



static int x86_generic_jmpcc (jin_interpreter * jint, jin_operand * operands, size_t nops, uint64_t target_flag, uint64_t condition ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    if ( (flags & target_flag) == condition )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}


	
instruction_handler x86_ins_ja (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_CF | X86_EFLAG_ZF, 0 );
}	
instruction_handler x86_ins_jae (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_CF, 0 );
}

instruction_handler x86_ins_jb (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_CF, X86_EFLAG_CF );
}	
instruction_handler x86_ins_jbe (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_CF | X86_EFLAG_ZF, X86_EFLAG_CF | X86_EFLAG_ZF );
}

instruction_handler x86_ins_je (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_ZF, X86_EFLAG_ZF );
}
instruction_handler x86_ins_jne (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_ZF, 0 );
}

instruction_handler x86_ins_jo (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_OF, X86_EFLAG_OF );
}
instruction_handler x86_ins_jno (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_OF, 0 );
}

instruction_handler x86_ins_jp (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_PF, X86_EFLAG_PF );
}
instruction_handler x86_ins_jnp (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_PF, 0 );
}

instruction_handler x86_ins_js (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_SF, X86_EFLAG_SF );
}
instruction_handler x86_ins_jns (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return x86_generic_jmpcc (jint, operands, nops, X86_EFLAG_SF, 0 );
}

instruction_handler x86_ins_jg (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == 0 ) || ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == (X86_EFLAG_SF | X86_EFLAG_OF) )
        && ( (flags & X86_EFLAG_ZF) == 0 ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}

instruction_handler x86_ins_jge (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == 0 ) || ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == (X86_EFLAG_SF | X86_EFLAG_OF) ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}

instruction_handler x86_ins_jle (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != 0 ) && ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != (X86_EFLAG_SF | X86_EFLAG_OF) ) ) 
        || ( (flags & X86_EFLAG_ZF) == X86_EFLAG_ZF ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}

instruction_handler x86_ins_jl (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != 0 ) && ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != (X86_EFLAG_SF | X86_EFLAG_OF) ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}


instruction_handler x86_ins_jcxz (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags = 0;
    
    read_register(jint, X86_REG_CX, &flags);
    
    if ( flags == 0 )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}

instruction_handler x86_ins_jecxz (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags = 0;
    
    read_register(jint, X86_REG_ECX, &flags);
    
    if ( flags == 0 )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}

instruction_handler x86_ins_jrcxz (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src = operands[0];
    uint64_t flags = 0;
    
    read_register(jint, X86_REG_RCX, &flags);
    
    if ( flags == 0 )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return JIN_ERR_OK;
}


instruction_handler x86_ins_call( jin_interpreter * jint, jin_operand * operands, size_t nops ){
    if ( nops != 1 || jint == NULL )
        return JIN_ERR_GENERIC;

    jin_operand src_op = operands[0];
    jin_mode mode = jin_get_mode(jint);
    uint64_t new_ip;
    uint64_t old_ip;
    uint64_t sp;
    jin_err jerr;

    jerr = x86_read_operand(jint, src_op, &new_ip);
    if ( jerr != JIN_ERR_OK )
        return jerr;

    if( read_register( jint, X86_REG_RIP, &old_ip ) != QWORD ) return JIN_ERR_REG_CANNOT_READ;
    if( read_register( jint, X86_REG_RSP, &sp ) != QWORD ) return JIN_ERR_REG_CANNOT_READ;

    if      ( mode == JIN_MODE_32 ) sp -= DWORD;
    else if ( mode == JIN_MODE_64 ) sp -= QWORD;

    if( write_memory(jint, sp, &old_ip, src_op.size) != src_op.size ) return JIN_ERR_MEM_CANNOT_WRITE;
    if( write_register( jint, X86_REG_RIP, &new_ip ) != QWORD ) return JIN_ERR_REG_CANNOT_WRITE;
    if( write_register(jint, X86_REG_RSP, &sp) != QWORD ) return JIN_ERR_REG_CANNOT_WRITE;

    return JIN_ERR_OK;
}

instruction_handler x86_ins_ret ( jin_interpreter * jint, jin_operand * operands, size_t nops ){
    if ( nops != 0 || jint == NULL )    // In x86 manual ret can have 1 argument, but keystone does not support it
        return JIN_ERR_GENERIC;

    jin_mode mode = jin_get_mode(jint);
    uint64_t new_ip;
    uint64_t sp;
    uint64_t addr_size;

    if      ( mode == JIN_MODE_32 ) addr_size = DWORD;
    else    addr_size = QWORD;

    if( read_register( jint, X86_REG_RSP, &sp ) != QWORD ) return JIN_ERR_REG_CANNOT_READ;
    if( read_memory(jint, sp, &new_ip, addr_size) != addr_size ) return JIN_ERR_MEM_CANNOT_READ;

    sp += addr_size;

    if( write_register( jint, X86_REG_RIP, &new_ip ) != QWORD ) return JIN_ERR_REG_CANNOT_WRITE;
    if( write_register(jint, X86_REG_RSP, &sp) != QWORD ) return JIN_ERR_REG_CANNOT_WRITE;

    return JIN_ERR_OK;
}
