

#include "cmd_break.h"


cmd_handler cmd_break( jin_interpreter * jint, string operands) {
    string tok[2];
    jin_err jerr;
    memory_addr address;
    
    jerr = cmd_parse_operands(operands, tok, 2);
    if ( jerr != JIN_ERR_OK )
        return (cmd_handler) jerr;
    
    address = strtol(tok[1], NULL, 16);
    if ( address == 0 )
        if ( errno == EINVAL || errno == ERANGE )
            return JIN_ERR_CMD_INVALIDARGUMENT;
    
    if ( get_effective_pointer(jint, address) == NULL ){
        puts("The given address points to an unmapped memory region!");
        return JIN_ERR_CMD_INVALIDARGUMENT;
    }
    
    add_breakpoint(tok[0], address );
    void * f = iter_breakpoint;
    return JIN_ERR_OK;
}

cmd_handler cmd_delbreak( jin_interpreter * jint, string operands) {
    string tok[1];
    jin_err jerr;
    
    jerr = cmd_parse_operands(operands, tok, 1);
    if ( jerr != JIN_ERR_OK )
        return (cmd_handler) jerr;
    
    if ( del_breakpoint(tok[0]) < 0){
        printf("Breakpoint \“%s\“ does not exist!\n", tok[0]);
        return JIN_ERR_CMD_INVALIDARGUMENT;
    }
    
    return JIN_ERR_OK;
}