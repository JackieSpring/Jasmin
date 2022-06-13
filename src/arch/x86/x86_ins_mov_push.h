
#ifndef _JASMIN_X86_MOV_PUSH_H
#define _JASMIN_X86_MOV_PUSH_H

#include "x86_generic.h"

instruction_handler x86_ins_mov(jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_pop (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_push (jin_interpreter * jint, jin_operand * operands, size_t nops );

#endif