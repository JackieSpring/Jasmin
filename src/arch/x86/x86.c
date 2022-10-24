
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
        { puts("GOTO 1"); goto cleanup; }
    
    for ( unsigned int i = 0; i < raw_op_count; i++ ) {
    #define rop operands[i]
        jin_operand newop ;
        newop.size = rop.size;
        
        if ( rop.type == X86_OP_REG ){
            newop.type = JIN_OP_REG;
            newop.reg = rop.reg;
        }
        else if ( rop.type == X86_OP_IMM ) {
            newop.type = JIN_OP_IMM;
            newop.reg = rop.imm;
        }
        else if ( rop.type == X86_OP_MEM ) {
            
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
            {  puts("GOTO 2");  goto cleanup;  }
            
            newop.type = JIN_OP_MEM;
            newop.mem = addr;
            
            
        }
        else
            { puts("GOTO 3"); goto cleanup;}
        (* ret)[i] = newop;
        #undef rop
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

    uint64_t buffer;
    
// INIT registers                                  
    jin_add_register(jint, X86_REG_RAX, QWORD);
    jin_add_register(jint, X86_REG_RBX, QWORD);
    jin_add_register(jint, X86_REG_RCX, QWORD);
    jin_add_register(jint, X86_REG_RDX, QWORD);
    jin_add_register(jint, X86_REG_RDI, QWORD);
    jin_add_register(jint, X86_REG_RSI, QWORD);
    jin_add_register(jint, X86_REG_RBP, QWORD);
    jin_add_register(jint, X86_REG_RSP, QWORD);
    jin_add_register(jint, X86_REG_RIP, QWORD);
    jin_add_register(jint, X86_REG_EFLAGS, QWORD);
    
    jin_add_register_partition( jint, X86_REG_AL, X86_REG_RAX, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_BL, X86_REG_RBX, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_CL, X86_REG_RCX, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_DL, X86_REG_RDX, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_DIL, X86_REG_RDI, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_SIL, X86_REG_RSI, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_BPL, X86_REG_RBP, 0, BYTE );
    jin_add_register_partition( jint, X86_REG_SPL, X86_REG_RSP, 0, BYTE );
    
    jin_add_register_partition( jint, X86_REG_AH, X86_REG_RAX, BYTE, BYTE );
    jin_add_register_partition( jint, X86_REG_BH, X86_REG_RBX, BYTE, BYTE );
    jin_add_register_partition( jint, X86_REG_CH, X86_REG_RCX, BYTE, BYTE );
    jin_add_register_partition( jint, X86_REG_DH, X86_REG_RDX, BYTE, BYTE );
    
    jin_add_register_partition( jint, X86_REG_AX, X86_REG_RAX, 0, WORD );
    jin_add_register_partition( jint, X86_REG_BX, X86_REG_RBX, 0, WORD );
    jin_add_register_partition( jint, X86_REG_CX, X86_REG_RCX, 0, WORD );
    jin_add_register_partition( jint, X86_REG_DX, X86_REG_RDX, 0, WORD );
    jin_add_register_partition( jint, X86_REG_DI, X86_REG_RDI, 0, WORD );
    jin_add_register_partition( jint, X86_REG_SI, X86_REG_RSI, 0, WORD );
    jin_add_register_partition( jint, X86_REG_BP, X86_REG_RBP, 0, WORD );
    jin_add_register_partition( jint, X86_REG_SP, X86_REG_RSP, 0, WORD );
    
    jin_add_register_partition( jint, X86_REG_EAX, X86_REG_RAX, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_EBX, X86_REG_RBX, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_ECX, X86_REG_RCX, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_EDX, X86_REG_RDX, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_EDI, X86_REG_RDI, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_ESI, X86_REG_RSI, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_EBP, X86_REG_RBP, 0, DWORD );
    jin_add_register_partition( jint, X86_REG_ESP, X86_REG_RSP, 0, DWORD );
    
    buffer = 0x202;
    write_register(jint, X86_REG_EFLAGS, &buffer);

// init instructions  
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



