#ifndef _JASMIN_CMD_PRINT_H
#define _JASMIN_CMD_PRINT_H

#include <errno.h>
#include "commands_generic.h"
#include "breakpoint.h"

cmd_handler cmd_print( jin_interpreter * jint, string operands);
void cmd_print_help();

#endif