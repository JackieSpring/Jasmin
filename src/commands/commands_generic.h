
#pragma once


#include "shared_macros.h"
#include "jin_error.h"
#include "jin_string.h"
#include "interpreter.h"

#define COMMAND_PREFIX "<"
#define COMMAND_SEPARATOR " \n"
#define COMMAND_ARGUMENT_SEPARATOR " "

typedef jin_err (* cmd_handler) ( jin_interpreter * jint, string operands );


string cmd_identify_command_string( string str );
string cmd_parse_command( string str, string * operands_str ) ;
jin_err cmd_parse_operands(string operands, string * ret, size_t nops );