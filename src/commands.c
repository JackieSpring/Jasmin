#include "commands.h"


static cmd_handler cmd_exit( jin_interpreter * jint, string operands);
static cmd_handler cmd_help();


hashtable * commands_table;


jin_err init_commands() {
    commands_table  = create_hashtable();
    if ( commands_table == NULL )
        return JIN_ERR_GENERIC;
    
    add_to_hashtable(commands_table, "exit", cmd_exit);
    add_to_hashtable(commands_table, "help", cmd_help);
    add_to_hashtable(commands_table, "print", cmd_print);

    return JIN_ERR_OK;
}

void fini_commands(){
    destroy_hashtable(commands_table);
}





bool is_command( string str ) {
    if ( cmd_identify_command_string(str) != NULL )
        return true;
    return false;
}


jin_err execute_command(jin_interpreter * jint, string str ) {

    if(jint == NULL || str == NULL)
        return JIN_ERR_GENERIC;

    string operands;
    cmd_handler cmh;

    str = cmd_identify_command_string(str);
    str = cmd_parse_command(str, &operands);
    if ( str == NULL )
        return JIN_ERR_CMD_INVALID;
    
    if( get_value_hashtable( commands_table, str, &cmh ) < 0)
        return JIN_ERR_CMD_INVALID;
    
    return (cmh)(jint, operands);
}

static cmd_handler cmd_exit( jin_interpreter * jint, string operands) {
    jin_stop_interpreter(jint);
    return JIN_ERR_OK;
}

static cmd_handler cmd_help() {
    puts( "Commands syntax: <`command` `arguments` , ex:   < print memory 0xdead 2" );
    puts("Available commands:");
    puts("exit");
    puts("help");
    puts("print");
    cmd_print_help();
    
    return JIN_ERR_OK;
}

