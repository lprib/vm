#ifndef VM_BASE_TYPES_H
#define VM_BASE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef int16_t vm_int;
typedef uint16_t vm_uint;
typedef bool vm_bool;

typedef enum
{
    VM_PROCESS_CONTINUE,
    VM_PROCESS_PROGRAM_HALT,
    VM_PROCESS_ERROR_UNDEF_IO_FN
} vm_programTickResult_t;

// bitwise casts
#define VM_TO_SIGNED(n) (*(vm_int *)&n)
#define VM_TO_UNSIGNED(n) (*(vm_uint *)&n)

#endif
