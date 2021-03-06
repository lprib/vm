#ifndef VM_BASE_TYPES_H
#define VM_BASE_TYPES_H

#include <limits.h>
#include <stdbool.h>

typedef short vm_int;
typedef unsigned short vm_uint;

#define VM_INT_MAX SHRT_MAX
#define VM_INT_MIN SHRT_MIN
#define VM_UINT_MAX USHRT_MAX

#define VM_INT_BITWIDTH (sizeof(vm_int) * CHAR_BIT)

// Amount of bits to shift to get the high half of a vm_uint
#define VM_HIGH_PART_SHIFT 8
// Mask to separate low half of a vm_uint
#define VM_LOW_PART_MASK 0x00FF

// Pack low and high parts to single int
#define VM_PACK_TO_INT(high, low) ((high << VM_HIGH_PART_SHIFT) | low)

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
