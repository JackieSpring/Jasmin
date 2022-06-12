
#ifndef _JASMIN_X86_CPU_H
#define _JASMIN_X86_CPU_H

#include "x86_generic.h"

int x86_ins_hlt(jin_interpreter * jint, jin_operand * operands, size_t nops );
int x86_ins_nop(jin_interpreter * jint, jin_operand * operands, size_t nops );

#endif