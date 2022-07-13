#ifndef _JASMIN_ERROR_H
#define _JASMIN_ERROR_H

#include <errno.h>
#include <keystone/keystone.h>
#include <capstone/capstone.h>


#include "shared_macros.h"


typedef enum jin_err {
    JIN_ERR_GENERIC = -1,           // undefined error
    JIN_ERR_OK = 0,                 // no error
    
    JIN_ERR_OPT_INVALID,            // unsupported option
    JIN_ERR_OPERAND_FAIL,           // could not resolve operands
    JIN_ERR_MEMORY,                 // could not allocate system memory
    
    JIN_ERR_ASM_INVALIDOPERAND,
    JIN_ERR_ASM_MISSINGFEATURE,
    JIN_ERR_ASM_MNEMONICFAIL,
    
    JIN_ERR_REG_CANNOT_READ,
    JIN_ERR_REG_CANNOT_WRITE,
    JIN_ERR_REG_CREATE_FAIL,        // cannot add register
    
    JIN_ERR_MEM_CANNOT_READ,
    JIN_ERR_MEM_CANNOT_WRITE,
    JIN_ERR_INVALID_MEMMAP,         // invalid memory map
    
    JIN_ERR_INSN_FETCH_FAIL,        // could not fetch new instruction
    JIN_ERR_INSN_EXEC_FAIL,         // failed instruction execution
    JIN_ERR_INSN_DECODE_FAIL,       // could decode instruction
    JIN_ERR_INSN_NOTSUPPORTED,      // no instruction handler
    
    JIN_ERR_SYM_UNDEFINED,          // undefined symbol
    
    JIN_ERR_CMD_INVALID,
    JIN_ERR_CMD_FEW_ARGUMENTS,
    JIN_ERR_CMD_TOOMANYARGUMENTS,
    JIN_ERR_CMD_INVALIDARGUMENT,
    
    
} jin_err;

void jin_perror( jin_err err );
void jin_throw_error( jin_err err );

jin_err ks_to_jin_err(ks_err err);
jin_err cs_to_jin_err(cs_err err);

#endif