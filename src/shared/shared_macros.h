#ifndef _SHARED_MACROS_H
#define _SHARED_MACROS_H

#include <limits.h>
#include <sys/types.h>
#include <stdlib.h>

#if (SIZE_MAX < 0xffffffffffffffff)
    #undef size_t
    #define size_t uint64_t
#endif

#define _1KiB   1024
#define KiB(x) (_1KiB * x)
#define MiB(x) (KiB(1024) * x)

#define BYTE  1
#define WORD  2
#define DWORD 4
#define QWORD 8


#ifndef bool
    #define bool char
    #define true 1
    #define false 0
#endif

#if DEBUG_ON
    #include <stdio.h>
    #define DEBUG(...) (printf("%s ", __func__), printf( __VA_ARGS__), printf("\n"), fflush(stdout))
#else
    #define DEBUG(...)
#endif

#endif