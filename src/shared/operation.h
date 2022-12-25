#ifndef _JASMIN_OPERATION_H
#define _JASMIN_OPERATION_H

#include "shared_macros.h"

/**
 *  These functions performs the signed or unsigned operation between the first 'size` bytes of 'op1` and 'op2`, the result
 *  will be the same in byte size as the operands and will be stored in the buffer pointed by '* total`; if the result size exceeds
 *  the 'size` argument, the return value will be true, false otherwise.
 *  The 'size` parameter accepts only BYTE, WORD, DWORD or QWORD values, if an invalid value is passed to the function,
 *  no calculation will be performed and the return value will be false.
 *  @param op1      first operator
 *  @param op2      second operator
 *  @param size     byte size of the operators (BYTE, WORD, DWORD or QWORD)
 *  @param total    pointer to the result
 *
 *  @return         true if there were any carry bits, false otherwise
 */

bool unsigned_add_overflow( uint64_t op1, uint64_t op2, size_t size, uint64_t * total );
bool signed_add_overflow( int64_t op1, int64_t op2, size_t size, int64_t * total );

bool unsigned_sub_overflow( uint64_t op1, uint64_t op2, size_t size, uint64_t * total );
bool signed_sub_overflow( int64_t op1, int64_t op2, size_t size, int64_t * total );

bool unsigned_mul_overflow( uint64_t op1, uint64_t op2, size_t size, uint64_t * total );
bool signed_mul_overflow( int64_t op1, int64_t op2, size_t size, int64_t * total );

#endif
