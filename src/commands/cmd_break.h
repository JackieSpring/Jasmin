#ifndef _JASMIN_CMD_BREAK_H
#define _JASMIN_CMD_BREAK_H

#include <errno.h>
#include "commands_generic.h"
#include "breakpoint.h"

cmd_handler cmd_break( jin_interpreter * jint, string operands);
cmd_handler cmd_delbreak( jin_interpreter * jint, string operands);

#endif