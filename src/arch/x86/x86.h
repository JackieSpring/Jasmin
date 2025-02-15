#ifndef _JASMIN_X86_H
#define _JASMIN_X86_H

#include <capstone/x86.h>

#include "interpreter.h"
#include "shared_macros.h"
#include "operation.h"

#include "x86_generic.h"
#include "x86_ins_operation_logic.h"
#include "x86_ins_mov_push.h"
#include "x86_ins_jmp_call.h"
#include "x86_ins_cpu.h"


int init_x86( jin_interpreter * jint);

#endif