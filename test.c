
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include <getopt.h>

#include "interpreter.h"
#include "shared_macros.h"
#include "jin_error.h"
#include "x86.h"
#include "commands.h"

#include "breakpoint.h"

#define CMD_CURSOR " o> "



/*
    Current running interpreter
*/ 
jin_interpreter * jint;


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


static unsigned char * fetch_command_line( jin_interpreter * jint, memory_addr ip ){
    const unsigned char * bytecode;
    unsigned char asmcode[256];
    unsigned char * code_ptr;
    unsigned char * label_ptr;
    size_t label_size;
    size_t size;
    size_t nins;
    jin_err jerr;
    memory_addr memptr;
    
    do {
        printf(CMD_CURSOR);
        fgets(asmcode, 256, stdin);
        asmcode[ sizeof(asmcode) - 1 ] = '\0';
        
        code_ptr = str_trim( asmcode );

        if ( is_command(code_ptr) == true ) {
            jerr = execute_command(jint, code_ptr);
            if( jerr != JIN_ERR_OK)
                jin_perror(jerr);
            continue;
        }
        
        code_ptr = contains_label(code_ptr, &label_ptr, &label_size);
        
        if ( label_size != 0 ) {
            label_ptr[label_size] = '\0';
            jin_add_symbol(jint, label_ptr, ip);
        }
    
        if ( strlen(code_ptr) < 2 )
            continue;
        
        break;
        
    } while ( jin_get_state(jint) != JIN_STATE_TERMINATED );
    
    if ( jin_get_state(jint) == JIN_STATE_TERMINATED )
        return NULL;
    
    
    
    jerr = jin_assemble(jint, code_ptr, ip, &bytecode, &size, &nins);
    if ( jerr != JIN_ERR_OK )
        goto cleanup;

    
    memptr = push_segment( jint, MEM_SEG_TEXT, bytecode, size );
    if ( memptr == MEM_FAILURE ) {
        jerr = JIN_ERR_MEM_CANNOT_WRITE;
        goto cleanup;
    }
    
    free(bytecode);
    
    return (const char *)get_effective_pointer( jint, memptr);
    
cleanup:
    jin_perror(jerr);
    if ( bytecode != NULL )
        free(bytecode);
    return NULL;
}

/*
 * NOTE:
 * La fase di fetch varia pesantemente a seconda della modalità di avviamento dell'interprete, inoltre è strettamente dipendente
 * dal valore dell'IP:
 *  se avviato in modalità interattiva, l'interprete deve essere in grado di capire se IP sta puntando a 
 *      - istruzioni inizializzate
 *          in questo caso deve ritornare le istruzioni senza richiedere input
 *      - testa del text-segment
 *          allora deve richiedere una nuova istruzione da inizializzare
 *      - oltre la testa del text-segment
 *          allora deve fallire
 *
 *  se avviato in modalità non-interattiva, allora deve popolare il text-segment all'avviamento e successivamente recuperare
 *  l'istruzione puntata da IP senza inizializzarne di nuove.
 *
*/
const unsigned char * fetch( jin_interpreter * jint ){
    memory_addr ip = 0;
    memory_addr toptext = push_segment(jint, MEM_SEG_TEXT, NULL, 0);
    const char * ret;
    
    read_instruction_pointer(jint, &ip);
    
    if ( ip == toptext ){
        ret = fetch_command_line(jint, ip);
    }
    else if ( ip >= offset_read_segment(jint, MEM_SEG_TEXT, 0, NULL, 0) && ip < toptext ) {
        ret = get_effective_pointer(jint, ip);
    }
    else {
        printf("UNKNOWN CASE toptext: %p, ip: %p\n", toptext, ip);
        exit(0);    
    }
    
    return ret;

cleanup:
    return NULL;
}

/*
 * NOTE:
 * Il modo in cui gli operandi devono essere interpretati è specifico dell'architettura, così come l'IP
*/
jin_instruction * decode( jin_interpreter * jint, const char * raw_code ) {
    uint64_t ip = 0x0;                
    size_t fake_size = sizeof(uint64_t) * 2 ;
    jin_instruction * ret = NULL;
    jin_err jerr;
    
    read_instruction_pointer(jint, &ip);
    
    jerr = jin_disassemble(jint, ip, raw_code, fake_size, &ret);
    if( jerr != JIN_ERR_OK )
        goto cleanup;
    ip += ret->byte_size;
    
    write_instruction_pointer(jint, &ip);
    
    return ret;
    
cleanup:
    jin_perror(jerr);
    
    if ( ret != NULL )
        jin_free_instruction(ret);
    
    return NULL;
}


int execute(jin_interpreter * jint, jin_instruction * ins ) {
    jin_err jerr = jin_execute(jint, ins);
    
    if ( jerr != JIN_ERR_OK ) {
        jin_perror( jerr );
        return -1;
    }
    
    return 0;
}


//
// ARCHITECTURE FUNCTIONS
//



//
// UTILITY
//

void print_instruction(jin_instruction * ins ){
    printf("id: %u\n", ins->id);
    printf("op_count %p\n", ins->op_count);
    printf("operands_p %p\n", ins->op);
    printf("mnemonic: %s\n", ins->mnemonic);
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


int main(){

    jin_options jops = {
        .arch = JIN_ARCH_X86,
        .mode = JIN_MODE_64,
    };

    jint = jin_init_interpreter( &jops );
    assert(jint != NULL);


    assert( init_commands() == JIN_ERR_OK );
    
    jin_set_state(jint, JIN_STATE_RUNNING );
    
    //print_registers(jint);
    


	while ( jin_get_state(jint) != JIN_STATE_TERMINATED ) {
	
    	unsigned char * code = fetch(jint);
    	if ( code == NULL ) {
            jin_perror(JIN_ERR_INSN_FETCH_FAIL);
            puts("<<< Ignoring last input >>>");
            continue;
    	}
    	
    	jin_instruction * jins = decode(jint, code);
    	if ( jins == NULL ) {
        	jin_throw_error( JIN_ERR_INSN_DECODE_FAIL );
        	exit(EXIT_FAILURE);
    	}
    	
    	//print_instruction(jins);
    	if ( execute(jint, jins) < 0)
            jin_perror(JIN_ERR_INSN_EXEC_FAIL);
    	
    	
    	jin_free_instruction(jins);
        //print_registers(jint);
    
    }
    //print_registers(jint);
    
    fini_commands();
    jin_fini_interpreter(jint);
    
    
    return  0;
}



