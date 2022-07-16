#ifndef _JASMIN_COMMANDS_H
#define _JASMIN_COMMANDS_H

#include "commands_generic.h"
#include "cmd_break.h"
#include "cmd_print.h"

jin_err init_commands();
void fini_commands();

bool is_command(string str);
jin_err execute_command(jin_interpreter * jint, string str);


#endif