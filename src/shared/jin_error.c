#include "jin_error.h"

static const char * get_error_string( jin_err err ) {
    switch( err ) {
        default:
            return "Generic error occurred (JIN_ERR_GENERIC)";
        case JIN_ERR_OK :
            return "OK (JIN_ERR_OK)";
    
        case JIN_ERR_OPT_INVALID:
            return "Invalid option (JIN_ERR_OPT_INVALID)";
        case JIN_ERR_OPERAND_FAIL :
            return "Failed decoding operand (JIN_ERR_OPERAND_FAIL)";
        case JIN_ERR_MEMORY:
            return "Could not allocate system memory (JIN_ERR_MEMORY)";

        case JIN_ERR_ASM_INVALIDOPERAND :
            return "Invalid operand (JIN_ERR_ASM_INVALIDOPERAND)";
        case JIN_ERR_ASM_MISSINGFEATURE:
            return "Missing CPU feature (JIN_ERR_ASM_MISSINGFEATURE)";
        case JIN_ERR_ASM_MNEMONICFAIL:
            return "Invalid mnemonic (JIN_ERR_ASM_MNEMONICFAIL)";
    
        case JIN_ERR_REG_CANNOT_READ :
            return "Could not read from register (JIN_ERR_REG_CANNOT_READ)";
        case JIN_ERR_REG_CANNOT_WRITE :
            return "Could not write to the register (JIN_ERR_REG_CANNOT_WRITE)";
        case JIN_ERR_REG_CREATE_FAIL :
            return "Could not create the register (JIN_ERR_REG_CREATE_FAIL)";

        case JIN_ERR_MEM_CANNOT_READ :
            return "Could not read from memory address (JIN_ERR_MEM_CANNOT_READ)";
        case JIN_ERR_MEM_CANNOT_WRITE :
            return "Could not write to the memory address (JIN_ERR_MEM_CANNOT_WRITE)";
        case JIN_ERR_INVALID_MEMMAP :
            return "Invalid memory map (JIN_ERR_INVALID_MEMMAP)";

        case JIN_ERR_INSN_FETCH_FAIL :
            return "Failed fetching new instruction (JIN_ERR_INSN_FETCH_FAIL)";
        case JIN_ERR_INSN_EXEC_FAIL :
            return "Failed instruction execution (JIN_ERR_INSN_EXEC_FAIL)";
        case JIN_ERR_INSN_DECODE_FAIL :
            return "Failed decoding instruction (JIN_ERR_INSN_DECODE_FAIL)";

        case JIN_ERR_SYM_UNDEFINED :
            return "Undefined symbol (JIN_ERR_SYM_UNDEFINED)";
    }
}

void jin_perror( jin_err err ) {
    puts(get_error_string(err));
}

void jin_throw_error( jin_err err ) {
    jin_perror(err);
    exit(EXIT_FAILURE);
}


jin_err key_to_jin_err(ks_err err) {
    switch(err) {
        case KS_ERR_OK:
            return JIN_ERR_OK;
        case KS_ERR_NOMEM:
            return JIN_ERR_MEMORY;
        case KS_ERR_OPT_INVALID:
            return JIN_ERR_OPT_INVALID;
            
        case KS_ERR_ASM_INVALIDOPERAND:
            return JIN_ERR_ASM_INVALIDOPERAND;
            
        case KS_ERR_ASM_MISSINGFEATURE:
            return JIN_ERR_ASM_MISSINGFEATURE;
            
        case KS_ERR_ASM_MNEMONICFAIL:
            return JIN_ERR_ASM_MNEMONICFAIL;
        default:
            return JIN_ERR_GENERIC;
    }
}

jin_err cap_to_jin_err(cs_err err) {
    switch(err){
        case CS_ERR_OK:
            return JIN_ERR_OK;
        case CS_ERR_MEM:
            return JIN_ERR_MEMORY;
        case CS_ERR_OPTION:
            return JIN_ERR_OPT_INVALID;
        default:
            return JIN_ERR_GENERIC;
    }
}





