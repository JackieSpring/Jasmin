
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include <capstone/capstone.h>
#include <capstone/x86.h>
#include <keystone/keystone.h>

#include "registers.h"
#include "isa.h"
#include "memory.h"
#include "shared_macros.h"
#include "x86.h"

#define BASE 0

// Current running interpreter
jin_interpreter * jint;


static unsigned char * str_trim ( const unsigned char * str ) {
    return (str + (strspn(str, " \t") ));
}

static bool str_isalphanum(const unsigned char * str, size_t len) {
    for ( unsigned int i = 0; i < len; i++ ){
        if ( !isalnum(str[i]) )
            return false;
    }
    return true;
}

/*
 * Given a string, if it contains a label declaration '[a-zA-Z0-9]:` then returns
 * a pointer to the first character after the label, or the string pointer if none
 
*/
static unsigned char * contains_label( const unsigned char * str, unsigned char ** label ,size_t * length ) {
    size_t lab_len;
    unsigned char * ret = str;
    lab_len = strcspn(str, ":");
    if ( str_isalphanum(str, lab_len) == true ) {
        ret = str_trim(str + lab_len + 1);
        *label = str;
        *length = lab_len;
    }
    else {
        *label = NULL;
        *length = 0;
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
    ks_err kerr;
    memory_addr codeptr;
    bool wait_input = true;
    
    do{
        printf(" > ");
        fgets(asmcode, 256, stdin);
        asmcode[ sizeof(asmcode) - 1 ] = '\0';
    
        wait_input = false;
        
        code_ptr = str_trim( asmcode );
        code_ptr = contains_label(code_ptr, &label_ptr, &label_size);
        if ( label_size != 0 ) {
            label_ptr[label_size] = '\0';
            add_symbol_to_table(jint->symt, label_ptr, ip);
        }
        
        if ( strlen(code_ptr) < 2 )
            wait_input = true;
        
    } while (wait_input == true);
    
    
    kerr = ks_asm(jint->ks, code_ptr, ip, &bytecode, &size, &nins);
    if (kerr != KS_ERR_OK)
        goto cleanup;
    
    codeptr = push_text( jint->mem, bytecode, size );
    if ( codeptr == MEM_FAILURE )
        goto cleanup;
    
    ks_free(bytecode);
    
    return (const char *)get_real_memory_pointer( jint->mem, codeptr);
cleanup:
    puts(ks_strerror( ks_errno(jint->ks) ) );
    
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
    memory_addr toptext = push_text(jint->mem, NULL, 0);
    const char * ret;
    
    read_instruction_pointer(jint, &ip);
    
    if ( ip == toptext ){
        ret = fetch_command_line(jint, ip);
    }
    else if ( ip < toptext && ip >= offset_read_text(jint->mem, 0, NULL, 0) ) {
        ret = get_real_memory_pointer(jint->mem, ip);
    }
    else
        goto cleanup;
    
    return ret;

cleanup:
    return NULL;
}

/*
 * NOTE:
 * Il modo in cui gli operandi devono essere interpretati è specifico dell'architettura, così come l'IP
*/
jin_instruction * decode( jin_interpreter * jint, const char * raw_code ) {
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // LEGGITI LA DOC DI cs_disasm_iter !!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    cs_insn * insn_meta = cs_malloc(jint->cs);      // è inutile ricreare l'istruzioe ogni volta, potresti farlo una volta sola e salvarti il puntatore
    uint64_t ip = 0x0;                // QUesto valore viene aggiornato da cs_disasm_iter per puntare all'istruzione succesiva, potrebbe essere sfruttato per aggioranre RIP
    size_t fake_size = sizeof(uint64_t)*2;    // 
    jin_instruction * ret = NULL;
    
    read_instruction_pointer(jint, &ip);
    
    if ( insn_meta == NULL )
        goto cleanup;
    
    if ( cs_disasm_iter( jint->cs, &raw_code, &fake_size, &ip, insn_meta ) == false )
        goto cleanup;
    
    ret = calloc( 1 , sizeof(jin_instruction));
    if ( ret == NULL )
        goto cleanup;
    
    ret->id = insn_meta->id;
    ret->byte_size = insn_meta->size;
    
    if( resolve_operands (jint, insn_meta, &ret->op, &ret->op_count ) < 0 )
        goto cleanup;
    
    cs_free(insn_meta, 1);
    
    write_instruction_pointer(jint, &ip);
    
    return ret;
    
cleanup:
    puts(cs_strerror( cs_errno(jint->cs) ) );
    if ( insn_meta != NULL )
        cs_free(insn_meta, 1);
    if ( ret != NULL )
        free(ret);
    return NULL;
}


int execute(jin_interpreter * jint, jin_instruction * ins ) {
    if ( jint == NULL || ins == NULL )
        goto cleanup;
    
    instruction_handler handler = (instruction_handler *)get_instruction_function( jint->isa, ins->id ) ;
    if ( handler == NULL )
        goto cleanup;
    
    if ( (handler)( jint, ins->op, ins->op_count ) < 0)
        goto cleanup;
    
    return 0;

cleanup:
    return -1;
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



static ks_sym_resolver symres(const char * sym, uint64_t * value){
    if( get_symbol_value( jint->symt, sym, value ) < 0)
        return false;
    
    return true;
}



int main(){

    //jint = jin_init_interpreter( JIN_ARCH_X86 , JIN_MODE_64 );
    jint = jin_init_interpreter( JIN_ARCH_X86 , JIN_MODE_32 );
    assert(jint != NULL);
    //ks_option(jint->ks, KS_OPT_SYNTAX, KS_OPT_SYNTAX_GAS | KS_OPT_SYNTAX_RADIX10 );
    ks_option(jint->ks, KS_OPT_SYM_RESOLVER, symres);
    
    
    init_x86(jint);
    jin_start_interpreter(jint);

	
    print_registers(jint);
	while ( jin_is_running(jint) ) {
	
    	unsigned char * code = fetch(jint);
    	if ( code == NULL ) {
        	puts("### FETCH ERROR: ignoring last input");
            continue;
    	}
    	
    	jin_instruction * jins = decode(jint, code);
    	assert(jins != NULL);
    	
    	if ( execute(jint, jins) < 0)
            puts(" !!!! FAILED EXECUTION !!!!");
    	
    	
    	jin_free_instruction(jins);
        //print_registers(jint);
    
    }
    print_registers(jint);
    
    jin_fini_interpreter(jint);
    
    
    return  0;
}



