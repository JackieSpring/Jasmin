
#include "x86.h"




static int x86_ins_fake_jump (jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( nops != 1 || jint == NULL )
        return -1;

    jin_operand src = operands[0];
    
    write_register(jint, X86_REG_RIP, &src.mem);
    return 0;
}


static int x86_operand_resolver (jin_interpreter * jint, cs_insn * raw_ins, jin_operand ** ret, size_t ** ret_op_count ){
    cs_x86_op * operands = (raw_ins->detail->x86).operands;
    uint8_t raw_op_count = (raw_ins->detail->x86).op_count;
    
    *ret = calloc( raw_op_count, sizeof(jin_operand) );
    
    
    if ( ret == NULL )
        goto cleanup;
    
    for ( unsigned int i = 0; i < raw_op_count; i++ ) {
        jin_operand newop ;
        newop.type = operands[i].type;
        newop.size = operands[i].size;
        
        if ( newop.type == X86_OP_REG )
            newop.reg = operands[i].reg;
        else if ( newop.type == X86_OP_IMM )
            newop.reg = operands[i].imm;
        else if ( newop.type == X86_OP_MEM ) {
            #define rop operands[i]
            // segment + base + index*scale + disp
            memory_addr addr = 0;
            memory_addr buffer = 0;

            if ( rop.mem.segment != X86_REG_INVALID ) {
                buffer = 0;
                read_register( jint, rop.mem.segment, &buffer );
                addr += buffer;
            }
            
            if ( rop.mem.base != X86_REG_INVALID ){
                buffer = 0;
                read_register( jint, rop.mem.base, &buffer );
                addr += buffer;
            }
            
            if ( rop.mem.index != X86_REG_INVALID ){
                buffer = 0;
                read_register( jint, rop.mem.index, &buffer );
                addr += buffer * rop.mem.scale;
            }
            
            if ( jin_get_mode(jint) == JIN_MODE_64 ) {
                addr += (signed) rop.mem.disp;
            } else if ( jin_get_mode(jint) == JIN_MODE_32 ) {
                addr += (signed int) rop.mem.disp;
            } else
                goto cleanup;
            
            newop.mem = addr;
            #undef rop
            
        }
        else
            goto cleanup;
        (* ret)[i] = newop;
    }
    
    *ret_op_count = raw_op_count;
    
    return 0;

cleanup:
    return -1;
}

//static register_size x86_instruction_pointer_reader_segment( jin_interpreter * jint, void * buffer );
//static register_size x86_instruction_pointer_writer_segment( jin_interpreter * jint, void * buffer ) {
static register_size x86_instruction_pointer_reader_direct( jin_interpreter * jint, void * buffer ) {
    if ( jint == NULL || buffer == NULL )
        return 0;
    
    return read_register(jint, X86_REG_RIP, buffer);
}
static register_size x86_instruction_pointer_writer_direct( jin_interpreter * jint, void * buffer ) {
    if ( jint == NULL || buffer == NULL )
        return 0;
    
    return write_register(jint, X86_REG_RIP, buffer);
}
//static register_size x86_stack_pointer_reader_segment( jin_interpreter * jint, void * buffer );
//static register_size x86_stack_pointer_writer_segment( jin_interpreter * jint, void * buffer ) {
static register_size x86_stack_pointer_reader_direct( jin_interpreter * jint, void * buffer ) {
    if ( jint == NULL || buffer == NULL )
        return 0;
    
    return read_register(jint, X86_REG_RSP, buffer);
}
static register_size x86_stack_pointer_writer_direct( jin_interpreter * jint, void * buffer ) {
    if ( jint == NULL || buffer == NULL )
        return 0;
    
    return write_register(jint, X86_REG_RSP, buffer);
}

/*
 * INITIALIZER
*/
int init_x86( jin_interpreter * jint) {

    if ( jin_create_register_table( jint, X86_REG_ENDING ) < 0)
        goto cleanup ;
    
    if ( jin_create_instruction_table(jint, X86_INS_ENDING) < 0 )
        goto cleanup ;

    // registers: A, B, C, D, DI, SI, BP, SP, IP, FLAGS
    // size 8*9
    void * regs_memory;
    uint64_t buffer;
    
    regs_memory = jin_malloc( jint, 11 * QWORD );
    
    assert( regs_memory != NULL );
    
    #define REG_A regs_memory
    #define REG_B regs_memory + QWORD
    #define REG_C regs_memory + (QWORD * 2 )
    #define REG_D regs_memory + (QWORD * 3 )
    #define REG_DI regs_memory + (QWORD * 4 )
    #define REG_SI regs_memory + (QWORD * 5 )
    #define REG_BP regs_memory + (QWORD * 6 )
    #define REG_SP regs_memory + (QWORD * 7 )
    #define REG_IP regs_memory + (QWORD * 8 )
    #define REG_FLAGS regs_memory + (QWORD * 9 )
    
    jin_add_register(jint, X86_REG_AL, BYTE , REG_A );
    jin_add_register(jint, X86_REG_AH, BYTE, REG_A + BYTE );
    jin_add_register(jint, X86_REG_AX, WORD, REG_A );
    jin_add_register(jint, X86_REG_EAX, DWORD, REG_A );
    jin_add_register(jint, X86_REG_RAX, QWORD, REG_A );
    
    jin_add_register(jint, X86_REG_BL, BYTE, REG_B );
    jin_add_register(jint, X86_REG_BH, BYTE, REG_B + BYTE );
    jin_add_register(jint, X86_REG_BX, WORD, REG_B );
    jin_add_register(jint, X86_REG_EBX, DWORD, REG_B );
    jin_add_register(jint, X86_REG_RBX, QWORD, REG_B );
    
    jin_add_register(jint, X86_REG_CL, BYTE, REG_C );
    jin_add_register(jint, X86_REG_CH, BYTE, REG_C + BYTE );
    jin_add_register(jint, X86_REG_CX, WORD, REG_C );
    jin_add_register(jint, X86_REG_ECX, DWORD, REG_C );
    jin_add_register(jint, X86_REG_RCX, QWORD, REG_C );
    
    jin_add_register(jint, X86_REG_DL, BYTE, REG_D );
    jin_add_register(jint, X86_REG_DH, BYTE, REG_D + BYTE );
    jin_add_register(jint, X86_REG_DX, WORD, REG_D );
    jin_add_register(jint, X86_REG_EDX, DWORD, REG_D );
    jin_add_register(jint, X86_REG_RDX, QWORD, REG_D );
    
    jin_add_register(jint, X86_REG_DIL, BYTE, REG_DI );
    jin_add_register(jint, X86_REG_DI, WORD, REG_DI );
    jin_add_register(jint, X86_REG_EDI, DWORD, REG_DI );
    jin_add_register(jint, X86_REG_RDI, QWORD, REG_DI );
    
    jin_add_register(jint, X86_REG_SIL, BYTE, REG_SI );
    jin_add_register(jint, X86_REG_SI, WORD, REG_SI );
    jin_add_register(jint, X86_REG_ESI, DWORD, REG_SI );
    jin_add_register(jint, X86_REG_RSI, QWORD, REG_SI );
    
    jin_add_register(jint, X86_REG_BPL, BYTE, REG_BP );
    jin_add_register(jint, X86_REG_BP, WORD, REG_BP );
    jin_add_register(jint, X86_REG_EBP, DWORD, REG_BP );
    jin_add_register(jint, X86_REG_RBP, QWORD, REG_BP );
    
    jin_add_register(jint, X86_REG_SPL, BYTE, REG_SP );
    jin_add_register(jint, X86_REG_SP, WORD, REG_SP );
    jin_add_register(jint, X86_REG_ESP, DWORD, REG_SP );
    jin_add_register(jint, X86_REG_RSP, QWORD, REG_SP );
    
    jin_add_register(jint, X86_REG_IP, WORD, REG_IP );
    jin_add_register(jint, X86_REG_EIP, DWORD, REG_IP );
    jin_add_register(jint, X86_REG_RIP, QWORD, REG_IP );

    jin_add_register(jint, X86_REG_EFLAGS, QWORD, REG_FLAGS );
    
    buffer = 0x202;
    write_register(jint, X86_REG_EFLAGS, &buffer);

    #undef REG_A 
    #undef REG_B 
    #undef REG_C 
    #undef REG_D 
    #undef REG_DI 
    #undef REG_SI 
    #undef REG_BP 
    #undef REG_SP 
    #undef REG_IP
    #undef REG_FLAGS
    
    jin_add_instruction(jint, X86_INS_ADD, x86_ins_add);
    jin_add_instruction(jint, X86_INS_AND, x86_ins_and);
    jin_add_instruction(jint, X86_INS_DEC, x86_ins_dec);
    jin_add_instruction(jint, X86_INS_HLT, x86_ins_hlt);
    jin_add_instruction(jint, X86_INS_INC, x86_ins_inc);
    
    jin_add_instruction(jint, X86_INS_JA, x86_ins_ja);
    jin_add_instruction(jint, X86_INS_JAE, x86_ins_jae);
    jin_add_instruction(jint, X86_INS_JB, x86_ins_jb);
    jin_add_instruction(jint, X86_INS_JBE, x86_ins_jbe);
    jin_add_instruction(jint, X86_INS_JE, x86_ins_je);
    jin_add_instruction(jint, X86_INS_JNE, x86_ins_jne);
    jin_add_instruction(jint, X86_INS_JP, x86_ins_jp);
    jin_add_instruction(jint, X86_INS_JNP, x86_ins_jnp);
    jin_add_instruction(jint, X86_INS_JS, x86_ins_js);
    jin_add_instruction(jint, X86_INS_JNS, x86_ins_jns);
    jin_add_instruction(jint, X86_INS_JG, x86_ins_jg);
    jin_add_instruction(jint, X86_INS_JGE, x86_ins_jge);
    jin_add_instruction(jint, X86_INS_JL, x86_ins_jl);
    jin_add_instruction(jint, X86_INS_JLE, x86_ins_jle);
    jin_add_instruction(jint, X86_INS_JCXZ, x86_ins_jcxz);
    jin_add_instruction(jint, X86_INS_JECXZ, x86_ins_jecxz);
    jin_add_instruction(jint, X86_INS_JRCXZ, x86_ins_jrcxz);
    
    jin_add_instruction(jint, X86_INS_LEA, x86_ins_lea);     
    jin_add_instruction(jint, X86_INS_MOV, x86_ins_mov);        
    jin_add_instruction(jint, X86_INS_MOVABS, x86_ins_mov);
    jin_add_instruction(jint, X86_INS_NOP, x86_ins_nop); 
    jin_add_instruction(jint, X86_INS_NOT, x86_ins_not);
    jin_add_instruction(jint, X86_INS_OR, x86_ins_or);    
    jin_add_instruction(jint, X86_INS_POP, x86_ins_pop);     
    jin_add_instruction(jint, X86_INS_PUSH, x86_ins_push);
    jin_add_instruction(jint, X86_INS_SUB, x86_ins_sub);
    jin_add_instruction(jint, X86_INS_XOR, x86_ins_xor);

    
    jin_add_instruction(jint, X86_INS_JMP, x86_ins_fake_jump);
    
    
    if ( jin_set_operand_resolver( jint, x86_operand_resolver ) < 0)
        goto cleanup;

    if ( jin_set_instruction_pointer_rw(jint, x86_instruction_pointer_reader_direct, x86_instruction_pointer_writer_direct) < 0 )
        goto cleanup;

    if ( jin_set_stack_pointer_rw(jint, x86_stack_pointer_reader_direct, x86_stack_pointer_writer_direct) < 0 )
        goto cleanup;

    return 0;

cleanup:
    return -1;

}



