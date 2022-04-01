#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#include "vm_BaseTypes.h"

// store the peek flag in the high bits of opcode
#define VM_PEEK_BITMASK (1 << (VM_INT_BITWIDTH - 1))

#define VM_JUMP_OPCODES \
    X(JUMPEQ, ==) \
    X(JUMPNEQ, !=) \
    X(JUMPLT, <) \
    X(JUMPGT, >) \
    X(JUMPLEQ, <=) \
    X(JUMPGEQ, >=)

#define VM_UNSIGNED_BINARY_OPCODES \
    X(ADD, l + r) \
    X(USUB, l - r) \
    X(UMUL, l * r) \
    X(DIV, l / r) \
    X(AND, l & r) \
    X(OR, l | r) \
    X(XOR, l ^ r) \
    X(MOD, l % r) \
    X(SHL, l << r) \
    X(LSHR, l >> r)

#define VM_SIGNED_BINARY_OPCODES \
    X(SSUB, l - r) \
    X(SMUL, l * r) \
    /* Even though the right parameter is technically unsigned, it is UB if */ \
    /* it overflows the bitwidth, so it's ok to cast to signed here */ \
    X(ASHR, l >> r) \

#define VM_OPCODE_UNARY_OPS \
    X(INC, n + 1) \
    X(DEC, n - 1) \
    X(INV, ~n)

// clang-format off
// disable format because X macros fuck with it

// Note size of enum MUST fit into bitwidth of vm_uint - 1
// to fit PEEK bit.
typedef enum
{
    // Memory
    VM_OPCODE_LOAD,
    VM_OPCODE_STORE,
    VM_OPCODE_LOAD_IMM,

    // Pointers
    VM_OPCODE_DEREF,
    VM_OPCODE_ARRAY_LOAD,
    VM_OPCODE_ARRAY_STORE,

    // Stack manipulation
    VM_OPCODE_PICK,
    VM_OPCODE_DUP,
    VM_OPCODE_SWAP,
    VM_OPCODE_DROP,

    // External interaction
    VM_OPCODE_IO,

    // control flow
    VM_OPCODE_HALT,
    VM_OPCODE_JUMP,
#define X(name, op) VM_OPCODE_##name,
    VM_JUMP_OPCODES

    // Operations
    VM_UNSIGNED_BINARY_OPCODES
    VM_SIGNED_BINARY_OPCODES
    VM_OPCODE_UNARY_OPS
#undef X

    VM_NUM_OPCODES

} vm_opcode_t;

// clang-format on

#endif
