#ifndef _JASMIN_CMD_SET_H
#define _JASMIN_CMD_SET_H

#include <errno.h>
#include "commands_generic.h"

cmd_handler cmd_set( jin_interpreter * jint, string operands);
void cmd_set_help();

#endif