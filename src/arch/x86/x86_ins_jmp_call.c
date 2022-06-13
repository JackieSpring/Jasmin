
#include "x86_ins_jmp_call.h"



static int x86_generic_jmpcc (jin_interpreter * jint, jin_operand * operands, size_t nops, uint64_t target_flag, uint64_t condition ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    if ( (flags & target_flag) == condition )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
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
        return -1;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == 0 ) || ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == (X86_EFLAG_SF | X86_EFLAG_OF) )
        && ( (flags & X86_EFLAG_ZF) == 0 ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}

instruction_handler x86_ins_jge (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == 0 ) || ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) == (X86_EFLAG_SF | X86_EFLAG_OF) ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}

instruction_handler x86_ins_jle (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != 0 ) && ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != (X86_EFLAG_SF | X86_EFLAG_OF) ) ) 
        || ( (flags & X86_EFLAG_ZF) == X86_EFLAG_ZF ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}

instruction_handler x86_ins_jl (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags;
    
    read_register(jint, X86_REG_EFLAGS, &flags);
    
    
    if ( ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != 0 ) && ((flags & (X86_EFLAG_SF | X86_EFLAG_OF)) != (X86_EFLAG_SF | X86_EFLAG_OF) ) )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}


instruction_handler x86_ins_jcxz (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags = 0;
    
    read_register(jint, X86_REG_CX, &flags);
    
    if ( flags == 0 )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}

instruction_handler x86_ins_jecxz (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags = 0;
    
    read_register(jint, X86_REG_ECX, &flags);
    
    if ( flags == 0 )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}

instruction_handler x86_ins_jrcxz (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    uint64_t flags = 0;
    
    read_register(jint, X86_REG_RCX, &flags);
    
    if ( flags == 0 )
        write_register(jint, X86_REG_RIP, &src.imm);
    
    return 0;
}

