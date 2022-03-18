#ifndef VM_BASETYPES_H
#define VM_BASETYPES_H

#include <stdint.h>

typedef int16_t vm_int;
typedef uint16_t vm_uint;

// bitwise casts
#define VM_TO_SIGNED(n) (*(vm_int *)&n)
#define VM_TO_UNSIGNED(n) (*(vm_uint *)&n)

#endif
