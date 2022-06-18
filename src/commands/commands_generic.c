
#include "commands_generic.h"

string cmd_identify_command_string( string str ){
    str = str_ltrim(str);
    str = strstr( str, COMMAND_PREFIX );
    
    if(str == NULL )
        return NULL;
    
    ++str;
    return  str_trim(str);
}

/*
 * Return a pointer to the first substring ending with a COMMAND_SEPARATOR, writes the 
 * pointer to the trailing string in operand_str
*/
string cmd_parse_command( string str, string * operands_str ) {

    if( str == NULL )
        return NULL;

    size_t index;
    string tok;
    
    tok = strtok( str, COMMAND_SEPARATOR );
    if ( tok == NULL ) {
        *operands_str = NULL;
        return str;
    }
    
    index = strlen(tok);
    index++;
    
    *operands_str = (str + index);
    
    return str;
}


jin_err cmd_parse_operands(string operands, string * ret, size_t nops ) {

    if( operands == NULL )
        return NULL;

    operands = str_trim(operands);
    
    if ( strlen(operands) != 0 && nops == 0 )
        return JIN_ERR_CMD_TOOMANYARGUMENTS;
    
    size_t i = 0;
    string tok;
    
    do{
    
        tok = strtok( operands, COMMAND_ARGUMENT_SEPARATOR );
        if ( tok == NULL )
            return JIN_ERR_CMD_FEW_ARGUMENTS;
        
        ret[i] = tok;
        
        operands = NULL;
        i++;
    
    } while ( i < nops );
    
    tok = strtok( NULL, COMMAND_ARGUMENT_SEPARATOR );
    if ( tok != NULL )
        return JIN_ERR_CMD_TOOMANYARGUMENTS;
    
    return JIN_ERR_OK;
}

