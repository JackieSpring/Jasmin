
#ifndef _JASMIN_X86_OPERATION_LOGIC_H
#define _JASMIN_X86_OPERATION_LOGIC_H

#include "x86_generic.h"

int x86_ins_add (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_and (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_dec (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_inc (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_lea (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_not (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_or  (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_sub (jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_xor (jin_interpreter * jint, jin_operand * operands, size_t nops );

#endif