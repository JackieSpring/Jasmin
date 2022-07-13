#include "commands.h"


hashtable * commands_table;





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
    jin_set_state(jint, JIN_STATE_TERMINATED);
    return JIN_ERR_OK;
}

static cmd_handler cmd_run ( jin_interpreter * jint, string operands) {
    memory_addr entrypoint = jin_get_entrypoint(jint);
    memory_addr stackbase = jin_get_stackbase(jint);
    
    if ( jin_get_state(jint) != JIN_STATE_INACTIVE ){
        puts("The interpreter is already running the programm");
        return JIN_ERR_OK;
    }
    write_instruction_pointer(jint, &entrypoint);
    write_stack_pointer(jint, &stackbase);
    jin_set_state( jint, JIN_STATE_RUNNING );
    
    return JIN_ERR_OK;
}

static cmd_handler cmd_continue ( jin_interpreter * jint, string operands) {
    if ( jin_get_state(jint) != JIN_STATE_PAUSE )
        puts("The programm is not paused, so it cannot continue");
    else
        jin_set_state(jint, JIN_STATE_RUNNING);
    
    return JIN_ERR_OK;
}

static cmd_handler cmd_pause ( jin_interpreter * jint, string operands) {
    if ( jin_get_state(jint) != JIN_STATE_RUNNING )
        puts("The programm is not running, so it cannot be paused");
    else
        jin_set_state(jint, JIN_STATE_PAUSE);
    
    return JIN_ERR_OK;
}

static cmd_handler cmd_term ( jin_interpreter * jint, string operands) {
    if ( jin_get_state(jint) != JIN_STATE_RUNNING || jin_get_state(jint) != JIN_STATE_PAUSE )
        puts("The programm is not running or paused, so it cannot be terminated");
    else
        jin_set_state(jint, JIN_STATE_INACTIVE);
    
    return JIN_ERR_OK;
}


static cmd_handler cmd_help() {
    puts( "Commands syntax: <`command` `arguments` , ex:   < print memory 0xdead 2" );
    puts("Available commands:");
    puts("continue");
    puts("exit");
    puts("help");
    puts("pause");
    puts("print");
    cmd_print_help();
    puts("run");
    puts("term");
    
    return JIN_ERR_OK;
}


jin_err init_commands() {
    commands_table  = create_hashtable();
    if ( commands_table == NULL )
        return JIN_ERR_GENERIC;
    
    add_to_hashtable(commands_table, "continue", cmd_continue);
    add_to_hashtable(commands_table, "exit", cmd_exit);
    add_to_hashtable(commands_table, "help", cmd_help);
    add_to_hashtable(commands_table, "pause", cmd_pause);
    add_to_hashtable(commands_table, "print", cmd_print);
    add_to_hashtable(commands_table, "run", cmd_run);
    add_to_hashtable(commands_table, "term", cmd_term);

    return JIN_ERR_OK;
}

void fini_commands(){
    destroy_hashtable(commands_table);
}

