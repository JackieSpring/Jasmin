
#include "cmd_set.h"

#define ADD_CMD_HANDLER(cmd_string, handler) if ( strcmp(set_routine, cmd_string) == 0 ) return handler(jint, arguments)

#define CMD_S_ENTRYP_ARG "entrypoint"
#define CMD_S_STACKB_ARG "stackbase"


static cmd_handler cmd_set_entrypoint( jin_interpreter * jint, string operands ){
    uint64_t address;
    string tok[1];
    jin_err jerr;

    jerr = cmd_parse_operands(operands, tok, 1);
    if( jerr != JIN_ERR_OK )
        return (cmd_handler) jerr;
    
    address = strtol(tok[0], NULL, 16);
    if ( address == 0 )
        if ( errno == EINVAL || errno == ERANGE )
            return JIN_ERR_CMD_INVALIDARGUMENT;
    
    jin_set_entrypoint(jint, address);

    if ( get_effective_pointer(jint, address) == NULL )
        puts( "-----! WARNING: Entrypoint points to an unmapped memory region !-----" );
    else {
        if ( check_perm_memory(jint, address, MEM_EXEC) == false )
            puts( "-----! WARNING: Entrypoint points to a non-executable memory region !-----");
        else if ( check_perm_memory(jint, address, MEM_READ) == false )
            puts( "-----! WARNING: Entrypoint points to a non-readable memory region !-----");
    }
    return JIN_ERR_OK;
}

static cmd_handler cmd_set_stackbase( jin_interpreter * jint, string operands ){
    uint64_t address;
    string tok;
    jin_err jerr;

    jerr = cmd_parse_operands(operands, tok, 1);
    if( jerr != JIN_ERR_OK )
        return (cmd_handler) jerr;
    
    address = strtol(tok[0], NULL, 16);
    if ( address == 0 )
        if ( errno == EINVAL || errno == ERANGE )
            return JIN_ERR_CMD_INVALIDARGUMENT;
    
    jin_set_stackbase(jint, address);

    if ( get_effective_pointer(jint, address) == NULL )
        puts( "-----! WARNING: Stackbase points to an unmapped memory region !-----" );
    else {
        if ( check_perm_memory(jint, address, MEM_WRITE) == false )
            puts( "-----! WARNING: Stackbase points to a non-witable memory region !-----");
        else if ( check_perm_memory(jint, address, MEM_READ) == false )
            puts( "-----! WARNING: Stackbase points to a non-readable memory region !-----");
    }
    return JIN_ERR_OK;
}



cmd_handler cmd_set( jin_interpreter * jint, string str){
    string arguments;
    string set_routine = cmd_parse_command(str, &arguments);

    if( set_routine == NULL )
        return JIN_ERR_CMD_INVALID;
    
    ADD_CMD_HANDLER( CMD_S_ENTRYP_ARG , cmd_set_entrypoint );
    ADD_CMD_HANDLER( CMD_S_STACKB_ARG , cmd_set_stackbase );

    return JIN_ERR_CMD_INVALID;

}


void cmd_set_help(){
    puts("  set "CMD_S_ENTRYP_ARG"\t\t\t" "set program entrypoint address");
    puts("  set "CMD_S_STACKB_ARG"\t\t\t\t" "set program stackbase address");
}