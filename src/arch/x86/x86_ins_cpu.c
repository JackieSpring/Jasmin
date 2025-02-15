
#include "x86_ins_cpu.h"


instruction_handler x86_ins_hlt(jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    if ( jint != NULL )
        jin_set_state(jint, JIN_STATE_INACTIVE);
    return 0;
}


instruction_handler x86_ins_nop(jin_interpreter * jint, jin_operand * operands, size_t nops ) {
    return 0;
}
