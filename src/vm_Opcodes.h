#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#include "vm_BaseTypes.h"

// store the peek flag in the high bits of opcode
#define VM_PEEK_BITMASK (1 << (VM_INT_BITWIDTH - 1))

#define VM_OPCODE_JUMPS \
    X(JUMPEQ, ==) \
    X(JUMPNEQ, !=) \
    X(JUMPLT, <) \
    X(JUMPGT, >) \
    X(JUMPLEQ, <=) \
    X(JUMPGEQ, >=)

#define VM_OPCODE_BINARY_OPS \
    X(ADD, l + r) \
    X(USUB, l - r) \
    X(SSUB, VM_TO_SIGNED(l) - VM_TO_SIGNED(r)) \
    X(UMUL, l * r) \
    X(SMUL, VM_TO_SIGNED(l) * VM_TO_SIGNED(r)) \
    X(DIV, l / r) \
    X(AND, l & r) \
    X(OR, l | r) \
    X(XOR, l ^ r) \
    X(MOD, l % r)

#define VM_OPCODE_UNARY_OPS \
    X(INC, n + 1) \
    X(DEC, n - 1) \
    X(INV, ~n)

// clang-format off
// disable format because X macros fuck with it

// Note size of enum MUST fit in a vm_uint!
typedef enum
{
    VM_OPCODE_LOAD,
    VM_OPCODE_STORE,
    VM_OPCODE_LOAD_IMM,
    VM_OPCODE_PICK,
    VM_OPCODE_DUP,
    VM_OPCODE_SWAP,
    VM_OPCODE_DROP,
    VM_OPCODE_JUMP,
    VM_OPCODE_IO,
    VM_OPCODE_HALT,

#define X(name, op) VM_OPCODE_##name,
    VM_OPCODE_JUMPS
    VM_OPCODE_BINARY_OPS
    VM_OPCODE_UNARY_OPS
#undef X

    VM_NUM_OPCODES

} vm_opcode_t;

// clang-format on

#endif
