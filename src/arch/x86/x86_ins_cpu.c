
#include "x86_ins_cpu.h"


int x86_ins_hlt(jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( jint != NULL )
        jin_stop_interpreter(jint);
    return 0;
}


int x86_ins_nop(jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return 0;
}
