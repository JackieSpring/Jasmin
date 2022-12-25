#include "commands.h"


hashtable * commands_table;



// Used in cmd_run
static register_iterator reset_registers ( register_id id, register_size size, bool is_partition, void * extra ) {
    if ( is_partition )
        return true;

    jin_interpreter * jint = (jin_interpreter *) extra;
    uint8_t zeros [size];
    bzero(zeros, size);
    
    write_register(jint, id, &zeros);

    return true;
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
    jin_set_state(jint, JIN_STATE_TERMINATED);
    return JIN_ERR_OK;
}

static cmd_handler cmd_run ( jin_interpreter * jint, string operands) {
    jin_iterate_register( jint, reset_registers, jint );

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
    if ( jin_get_state(jint) != JIN_STATE_RUNNING && jin_get_state(jint) != JIN_STATE_PAUSE )
        puts("The programm is not running or paused, so it cannot be terminated");
    else
        jin_set_state(jint, JIN_STATE_INACTIVE);
    
    return JIN_ERR_OK;
}


static cmd_handler cmd_help() {
    puts( "Commands syntax: <`command` `arguments` , ex:   < print memory 0xdead 2" );
    puts("------------ Available commands ------------");
    puts("break\t`name` `address`\t\tCreates a breakpoint at the given address, the breakpoint is ");
                     puts("\t\t\t\t\tidentified by the given name.");
    puts("continue\t      \t\t\tContinue execution from current Instruction Pointer");
    puts("delbreak\t`name`\t\t\tDeletes a breakpoint");
    puts("exit");
    puts("help");
    puts("pause   \t      \t\t\tPause code execution, new instructions will be pushed in the");
                puts("\t\t\t\t\tworking segment but not executed.");
    puts("print");
    cmd_print_help();
    puts("run     \t      \t\t\tStart execution from code entry point until last instruction or");
                puts("\t\t\t\t\tprogram end, new instructions will be pushed in the working segment");
                puts("\t\t\t\t\tand executed realtime.");
    puts("set ");
    cmd_set_help();
    puts("term    \t      \t\t\tTerminate code execution, new instructions will be pushed in");
                puts("\t\t\t\t\tthe working segment but not executed");
    puts("--------------------------------------------");
    
    return JIN_ERR_OK;
}


jin_err init_commands() {
    commands_table  = create_hashtable();
    if ( commands_table == NULL )
        return JIN_ERR_GENERIC;
    
    add_to_hashtable(commands_table, "break", cmd_break);
    add_to_hashtable(commands_table, "continue", cmd_continue);
    add_to_hashtable(commands_table, "delbreak", cmd_delbreak);
    add_to_hashtable(commands_table, "exit", cmd_exit);
    add_to_hashtable(commands_table, "help", cmd_help);
    add_to_hashtable(commands_table, "pause", cmd_pause);
    add_to_hashtable(commands_table, "print", cmd_print);
    add_to_hashtable(commands_table, "run", cmd_run);
    add_to_hashtable(commands_table, "set", cmd_set);
    add_to_hashtable(commands_table, "term", cmd_term);

    return JIN_ERR_OK;
}

void fini_commands(){
    destroy_hashtable(commands_table);
}

