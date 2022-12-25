#ifndef _JASMIN_X86_JMP_CALL_H
#define _JASMIN_X86_JMP_CALL_H

#include "x86_generic.h"

instruction_handler x86_ins_ja (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jae (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_jb (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jbe (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_je (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jne (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_jo (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jno (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_jp (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jnp (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_js (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jns (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_jg (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jge (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jl (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jle (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_jcxz (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jecxz (jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_jrcxz (jin_interpreter * jint, jin_operand * operands, size_t nops );

instruction_handler x86_ins_call( jin_interpreter * jint, jin_operand * operands, size_t nops );
instruction_handler x86_ins_ret( jin_interpreter * jint, jin_operand * operands, size_t nops );
#endif