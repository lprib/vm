#ifndef VM_BASE_TYPES_H
#define VM_BASE_TYPES_H

#include <stdbool.h>
#include <limits.h>

#define VM_INT_BITWIDTH 16

typedef short vm_int;
typedef unsigned short vm_uint;

#define VM_INT_MAX SHRT_MAX
#define VM_INT_MIN SHRT_MIN
#define VM_UINT_MAX USHRT_MAX

typedef enum
{
    VM_PROCESS_CONTINUE,
    VM_PROCESS_PROGRAM_HALT,
    VM_PROCESS_ERROR_UNDEF_IO_FN
} vm_programTickResult_t;

// eg: type_t UNUSED_P(paramName)
// shut up gcc about unused params
#ifdef __GNUC__
    #define UNUSED_P(x) UNUSED_##x __attribute__((__unused__))
#else
    #define UNUSED_P(x) UNUSED_##x
#endif

// eg: USE(paramName);
// shut up gcc about unused variables
#define UNUSED(x) (void)(x)

#endif
