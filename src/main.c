
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "interpreter.h"
#include "shared_macros.h"
#include "jin_error.h"
#include "x86.h"
#include "commands.h"

#include "breakpoint.h"

#define CMD_CURSOR " >>> "
#define USER_INPUT_SIZE 256


void print_instruction(jin_instruction * ins );
void print_operand(jin_operand * op);
void print_register(jin_interpreter *  jint, register_id id);
void print_registers( jin_interpreter * jint );



/*
 * Given a string, if it contains a label declaration '[a-zA-Z0-9]:` then returns
 * a pointer to the first character AFTER the label, or the string pointer if none
 
*/
static unsigned char * contains_label( const unsigned char * str, unsigned char ** label ,size_t * length ) {

    size_t lab_len;
    string ret;
    
    ret = strchr(str, ':');
    if ( ret == NULL ) {
        *label = NULL;
        *length = 0;
        return str;
    }
    lab_len = ret - str;
    
    if ( str_isalphanum(str, lab_len) == true ) {
        ret = str_ltrim(ret + 1);
        *label = str;
        *length = lab_len;
    }
    else {
        *label = NULL;
        *length = 0;
        ret = str;
    }
    
    return ret;
}


jin_err execute(jin_interpreter * jint) {
    uint64_t ip = 0x0;                
    size_t fake_size = sizeof(uint64_t) * 2 ;//
    jin_instruction * ins = NULL;
    memory_addr wseg_end;
    jin_err jerr;
    
    read_instruction_pointer(jint, &ip);
    wseg_end = push_segment( jint, jin_get_working_segment(jint), NULL, 0 );

    if ( (wseg_end - ip) < fake_size )  // do not fetch data outside memory boundaries (no heap leak)
        fake_size = (wseg_end - ip);
    
    // get_effective_pointer() does not check form memory permissions or boundaries
    jerr = jin_disassemble(jint, ip, get_effective_pointer(jint, ip) , fake_size, &ins);
    if( jerr != JIN_ERR_OK )
        goto cleanup;
        
    ip += ins->byte_size;
    write_instruction_pointer(jint, &ip);
    
    return jin_execute(jint, ins);
    
cleanup:
    if ( ins != NULL )
        jin_free_instruction(ins);
    
    return jerr;
}

jin_err push_code( jin_interpreter * jint, unsigned char * code_ptr ) {
    unsigned char * bytecode;
    size_t size;
    size_t nins;
    jin_err jerr;
    memory_addr memptr;

    jerr = jin_assemble(jint, code_ptr, push_segment( jint, jin_get_working_segment(jint), NULL , 0 ) , &bytecode, &size, &nins);   // assemble
    if ( jerr != JIN_ERR_OK ){
        if ( bytecode != NULL )
            free(bytecode);
        return jerr;
    }
    
    memptr = push_segment( jint, jin_get_working_segment(jint) , bytecode, size );  // push into working segment
    if ( memptr == MEM_FAILURE ) {
        if ( bytecode != NULL )
            free(bytecode);
        
        return JIN_ERR_MEM_CANNOT_WRITE;
    }
    free(bytecode);
    
    return jerr;
}

unsigned char * get_user_input( jin_interpreter * jint, unsigned char * user_input ) {
    string prefix;
    jin_state state = jin_get_state(jint);

    prefix = jin_state_to_string(state);
    
    printf("(%s)%s", prefix, CMD_CURSOR);
    fgets(user_input, USER_INPUT_SIZE, stdin);
    user_input[ USER_INPUT_SIZE - 1 ] = '\0';
    return str_trim( user_input );
}

void main_loop( jin_interpreter * jint ){

    unsigned char user_input[ USER_INPUT_SIZE ];
    unsigned char * code_ptr;
    unsigned char * label_ptr;
    size_t label_size;
    jin_err jerr;
    memory_addr memptr;
    memory_addr ip;
    memory_addr segment_top;
    segment_id working_segment;
    bool breakpoint_hit_flag = false;
    bool noexec_flag = false;


    while ( jin_get_state(jint) != JIN_STATE_TERMINATED ) {
        code_ptr = get_user_input( jint, user_input );

        if ( is_command(code_ptr) == true ) {                               // if is command, execute
            jerr = execute_command(jint, code_ptr);
            
            if( jerr != JIN_ERR_OK)
                jin_perror(jerr);
                
        }
        else {
            code_ptr = contains_label(code_ptr, &label_ptr, &label_size);       // if contains label, add label, then continue
            if ( label_size != 0 ) {
                label_ptr[label_size] = '\0';
                jin_add_symbol(jint, label_ptr, push_segment(jint, jin_get_working_segment(jint), NULL, 0) );
            }
            if ( strlen(code_ptr) < 2 )                                         // no instruction left
                continue;
            
            jerr = push_code(jint, code_ptr);
            if ( jerr != JIN_ERR_OK ){
                jin_perror(jerr);
                continue;
            }
        }
        
        working_segment = jin_get_working_segment(jint);

        if ( jin_get_state(jint) == JIN_STATE_INACTIVE )
            noexec_flag = false;

        if ( jin_get_state(jint) != JIN_STATE_RUNNING )                     // if the interpreter is not executing code, repeat
            continue;
        

        // If the IP already points to non-executable memory, continue
        if ( noexec_flag == true  )
            continue;

        // If the IP now points to non-executable memory, notify, then set STATE_PAUSE
        read_instruction_pointer(jint, &ip);
        if ( check_perm_memory( jint, ip, MEM_EXEC ) == false ) {
            jin_perror( JIN_ERR_MEM_CANNOT_EXEC );
            puts("-----! Instruction Pointer points to non-executable memory! !-----");
            noexec_flag = true;
            jin_set_state(jint, JIN_STATE_PAUSE);
            continue;
        }

        while( jin_get_state(jint) == JIN_STATE_RUNNING && 
            (   ip >= offset_read_segment(jint, working_segment , 0, NULL, 0) && 
                ip < push_segment(jint, working_segment, NULL, 0) 
            )
        ){
            if ( is_breakpoint(ip) && breakpoint_hit_flag == false ){                                           // if a breakpoint is declared on the address, pause execution
                breakpoint_hit_flag = true;
                jin_set_state(jint, JIN_STATE_PAUSE );
                puts("-----! Breakpoint hitted !-----");
                printf("Address: %p\n", ip );
                continue;
            }
            
            breakpoint_hit_flag = false;
            jerr = execute(jint);
            if( jerr != JIN_ERR_OK ){
                jin_perror(jerr);
                break;
            }
            read_instruction_pointer(jint, &ip);
            
        }

        // IP may points to non-executable memory, if so set STATE_PAUSE
        read_instruction_pointer(jint, &ip);
        if ( check_perm_memory( jint, ip, MEM_EXEC ) == false ){             // if the segment is not executable, repeat
            if ( noexec_flag == false ) {
                noexec_flag = true;
                puts("-----! Instruction Pointer points to non-executable memory! !-----");
                jin_set_state(jint, JIN_STATE_PAUSE);
            }
        }
    }
}


int main(int argc, char * argv[]){

    // -m   32 bit
    // -g   gas syntax

#define ARGS_OPTIONS "mg"

    int opt;
    jin_mode arg_mode = JIN_MODE_64;
    jin_syntax arg_syn = JIN_SYNTAX_INTEL;

    while ((opt = getopt(argc, argv, ARGS_OPTIONS)) != -1) {
        switch (opt) {
            case 'm': arg_mode = JIN_MODE_32; break;
            case 'g': arg_syn = JIN_SYNTAX_GAS; break;
        default:
            fprintf(stderr, "Usage: %s [-"ARGS_OPTIONS"] \n", argv[0]);
            fprintf(stderr, "\t-m\t\tUse x86 32bit mode \n");
            fprintf(stderr, "\t-g\t\tUse x86 GAS syntax \n");
            exit(EXIT_FAILURE);
        }
    }
#undef ARGS_OPTIONS

    jin_interpreter * jint;
    jin_options jops = {
        .arch = JIN_ARCH_X86,
        .mode = arg_mode,
        .syntax = arg_syn,
    };

    jint = jin_init_interpreter( &jops );
    assert( jint != NULL);
    assert( init_commands() == JIN_ERR_OK );
    
    jin_set_state(jint, JIN_STATE_RUNNING );

    puts("Starting Jasmin v1.0");
    puts("use <help for help");
    
    main_loop(jint);
    
    fini_commands();
    jin_fini_interpreter(jint);
    
    return 0;
}

//
// UTILITY
//

void print_instruction(jin_instruction * ins ){
    printf("id: %u\n", ins->id);
    printf("op_count %p\n", ins->op_count);
    printf("operands_p %p\n", ins->op);
    printf("mnemonic: %s\t%s\n", ins->ins_str, ins->op_str);
}

void print_operand(jin_operand * op) {
    DEBUG("op address %p", op);
    printf( "type: %p\n", op->type );
    printf( "imm: %p\n", op->imm);
    printf( "size: %p\n", op->size);
}

void print_register(jin_interpreter *  jint, register_id id) {
    uint64_t reg;
    read_register(jint, id, &reg);
    printf("%018p\n", reg);
}

void print_registers( jin_interpreter * jint ){

    printf("RAX: ");
    print_register(jint, X86_REG_RAX);
    printf("RBX: ");
    print_register(jint, X86_REG_RBX);
    printf("RCX: ");
    print_register(jint, X86_REG_RCX);
    printf("RDX: ");
    print_register(jint, X86_REG_RDX);
    printf("RDI: ");
    print_register(jint, X86_REG_RDI);
    printf("RSI: ");
    print_register(jint, X86_REG_RSI);
    printf("RBP: ");
    print_register(jint, X86_REG_RBP);
    printf("RSP: ");
    print_register(jint, X86_REG_RSP);
    printf("RIP: ");
    print_register(jint, X86_REG_RIP);
    printf("RFLAGS: ");
    print_register(jint, X86_REG_EFLAGS);

}