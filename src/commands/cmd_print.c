#include "cmd_print.h"

#define ADD_CMD_HANDLER(cmd_string, handler) if ( strcmp(print_routine, cmd_string) == 0 ) return handler(jint, arguments) \

/*
 * COMMANDS:
 * print break
 * print memory <address> <size>
 * print regs
 * print syms
*/

#define CMD_P_BREAK_ARG "break"
#define CMD_P_MEMORY_ARG "memory"
#define CMD_P_REGS_ARG "regs"
#define CMD_P_SYMS_ARG "syms"
#define CMD_P_CODE_ARG "code"
#define CMD_P_INT_ARG "interpreter"


//
// AUXILIARY FUNCTIONS
//

static void print_single_register ( string reg_name, void * data, register_size size ) {
    reg_name = str_trim(reg_name);
    printf("%-8s", reg_name);
    printf(" ");
    
    for ( ; size > 0 ; size-- )
        printf("%02x", ((uint8_t *)data)[size - 1] );
    
    puts("");
}

static void print_address_content_ascii( memory_addr addr, void * data, size_t len ) {
    // 0x0011223344556677  0011223344 0011223344 0011223344 0011223344  | 0123456789abcdef |
    size_t lines = len / 16;
    size_t trailing = len % 16;
    
    for ( size_t l = 0; l < lines; l++ ) {
        unsigned int i = 0;
        
        printf("%-016p", addr);
        printf("  ");
        
        for ( i = 0; i < 4 ; i++ ) {
            printf("%08x", ((uint32_t *) data)[i] );
            printf(" ");
        }
        
        printf(" | ");
        for ( i = 0; i < 16; i++ ) {
        
            if( isprint( ((uint8_t *) data)[i] ) )
                putchar( ((uint8_t *) data)[i] );
            else
                putchar('.');
        }
        printf(" |\n");
        
        addr += 16;
        data += 16;
        len -= 16;
    }
    
    if ( trailing != 0 ) {
        unsigned int i = 0;
        
        printf("%-016p", addr);
        printf("  ");
        
        for(unsigned int j = 0; j < 4; j++) {
        
            uint32_t * buff = &((uint32_t *) data)[j];
        
            for( i = 4 ; i > 0 ; i--) {
                if( trailing >= i ) {
                    printf("%02x", ((uint8_t *) buff)[i - 1] );
                    trailing--;
                }
                else
                    printf("  ");
            }
            printf(" ");
        }
        
        printf(" | ");
        for ( i = 0; i < 16; i++ ) {
            if ( len > 0 ) {
                if( isprint( ((uint8_t *) data)[i] ) )
                    putchar( ((uint8_t *) data)[i] );
                else
                    putchar('.');
                len--;
            }
        }
        printf(" |\n");
        
    }
}

static register_iterator iter_print_regs( register_id id, register_size size, bool is_partition, void * extra ){
    jin_interpreter * jint = (jin_interpreter *)extra;
    string reg_name;
    uint8_t buff[size];
    
    if ( is_partition == true )
        return true; // continue loop
    
    read_register( jint, id, buff );
    reg_name = jin_get_reg_name(jint, id);
    
    print_single_register( reg_name, buff, size );
    return true;
}

static symbol_iterator iter_print_syms( symbol_key key, symbol_value val, void * extra ) {
    printf("%-16s", key);
    printf("  ");
    printf("%p", val);
    puts("");
    return true;
}

static memory_iterator iter_print_code ( segment_id id, memory_addr start, size_t size, bool is_stack, void * extra ){
    jin_interpreter * jint = (jin_interpreter *)extra;
    memory_addr top = push_segment(jint, id, NULL, 0);
    unsigned char * bytecode;
    unsigned char * symbol;
    jin_instruction * ins;
    memory_addr code_ptr;
    jin_err jerr;
    
    if ( check_perm_memory( jint, start, MEM_EXEC ) == false || is_stack == true )
        return true;
        
    code_ptr = start;
    
    while ( code_ptr < top ){
    
        bytecode = get_effective_pointer(jint, code_ptr );
    
        jerr = jin_disassemble(jint, code_ptr, bytecode, sizeof(uint64_t) * 2 , &ins);
        if ( jerr != JIN_ERR_OK ) {
            puts("An error occurred while disassembling instructions");
            jin_perror(jerr);
            return false;
        }
        
        printf("%-016p", code_ptr);
        printf("\t");
        printf("%-16s", ins->ins_str);
        printf("%s", ins->op_str);
        puts("");
        
        code_ptr += ins->byte_size;
    }
    
    return true;
}

static bool iter_print_break ( breakpoint_id id, breakpoint_addr val,  void * extra ) {
    printf("%-16s", id);
    printf("  ");
    printf("%p", val);
    puts("");
    return true;
}

//
// SECOND LEVEL PRINT HANDLERS
//

/*
 * print memory <address> <size>    hexadecimal address, decimal size
 * prints a memory area in hex format
*/
static cmd_handler cmd_print_memory (jin_interpreter * jint, string operands) {
    uint64_t address, size;
    string tok [2];
    jin_err jerr;
    
    jerr = cmd_parse_operands(operands, tok, 2);
    if( jerr != JIN_ERR_OK )
        return jerr;
    
    address = strtol(tok[0], NULL, 16);
    if ( address == 0 )
        if ( errno == EINVAL || errno == ERANGE )
            return JIN_ERR_CMD_INVALIDARGUMENT;
            
    size = strtol(tok[1], NULL, 10);
    if ( address == 0 )
        if ( errno == EINVAL || errno == ERANGE )
            return JIN_ERR_CMD_INVALIDARGUMENT;

    uint8_t buffer[size];
    
    if( force_read_memory(jint, address, buffer, size) != size )
        return JIN_ERR_MEM_CANNOT_READ;
    
    print_address_content_ascii( address, buffer, size );
    
    return JIN_ERR_OK;
}

/*
 * print regs
 * print all defined registers
*/
static cmd_handler cmd_print_registers (jin_interpreter * jint, string operands) {
    jin_iterate_register( jint, iter_print_regs, jint );
    return JIN_ERR_OK;
}

/*
 * print syms
*/
static cmd_handler cmd_print_symbols (jin_interpreter * jint, string operands) {
    jin_iterate_symbol( jint, iter_print_syms, NULL );
    return JIN_ERR_OK;
}

static cmd_handler cmd_print_code (jin_interpreter * jint, string operands ){
    jin_iterate_memory(jint, iter_print_code, jint);
    return JIN_ERR_OK;
}

static cmd_handler cmd_print_break (){
    iter_breakpoint( iter_print_break, NULL );
    return JIN_ERR_OK;
}

static cmd_handler cmd_print_interpreter(jin_interpreter * jint, string operands ){
    uint64_t ip, sp;
    jin_arch arch = jin_get_arch(jint);
    jin_mode mode = jin_get_mode(jint);
    jin_syntax syn = jin_get_syntax(jint);
    jin_state state = jin_get_state(jint);

    read_instruction_pointer(jint, &ip);
    read_stack_pointer(jint, &sp);

    printf( "Architecture:\t\t%s\n",    jin_arch_to_string(arch) );
    printf( "Mode:\t\t\t%s\n",          jin_mode_to_string(arch, mode) );
    printf( "Syntax:\t\t\t%s\n",          jin_syntax_to_string(arch, syn));
    printf( "Entrypoint:\t\t%p\n",      jin_get_entrypoint(jint));
    printf( "Stackbase:\t\t%p\n",       jin_get_stackbase(jint));
    printf( "Workingsegment:\t\t%d\n",  jin_get_working_segment(jint));
    printf( "State:\t\t\t%s\n",         jin_state_to_string(state) );
    printf( "InstructionPointer:\t%p\n",ip );
    printf( "StackPointer:\t\t%p\n",    sp );

    return JIN_ERR_OK;
}

//
// MAIN HANDLER
//
cmd_handler cmd_print( jin_interpreter * jint, string str) {
    string arguments;
    string print_routine = cmd_parse_command(str, &arguments);
    
    if( print_routine == NULL )
        return JIN_ERR_CMD_INVALID;
    
    ADD_CMD_HANDLER( CMD_P_BREAK_ARG , cmd_print_break);
    ADD_CMD_HANDLER( CMD_P_MEMORY_ARG , cmd_print_memory);
    ADD_CMD_HANDLER( CMD_P_REGS_ARG , cmd_print_registers);
    ADD_CMD_HANDLER( CMD_P_SYMS_ARG , cmd_print_symbols);
    ADD_CMD_HANDLER( CMD_P_CODE_ARG , cmd_print_code );
    ADD_CMD_HANDLER( CMD_P_INT_ARG , cmd_print_interpreter );
    
    return JIN_ERR_CMD_INVALID;
}


void cmd_print_help() {
    puts("  print "CMD_P_BREAK_ARG"\t\t\t\t" "prints the current breakpoints declared");
    puts("  print "CMD_P_MEMORY_ARG" `address` `bytes` \t" "prints `bytes` bytes from the memory starting from `address`," );
    puts("                                        " "the address must be in hex format ex: 0xaabbccddee");
    puts("  print "CMD_P_REGS_ARG" \t\t\t\t" "prints the current registers and their content");
    puts("  print "CMD_P_SYMS_ARG" \t\t\t\t" "prints the current symbols and the address they point to");
    puts("  print "CMD_P_CODE_ARG" \t\t\t\t" "prints all the executable code written in memory");
    puts("  print "CMD_P_INT_ARG" \t\t\t" "prints interpreter infos");
}


