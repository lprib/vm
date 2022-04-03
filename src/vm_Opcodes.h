#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#include "vm_BaseTypes.h"

// store the peek flag in the high bits of opcode
#define VM_PEEK_BITMASK (1 << (VM_INT_BITWIDTH - 1))

// X(name, comparisionOperation)
#define VM_JUMP_OPCODES \
    X(JUMPEQ, ==) \
    X(JUMPNEQ, !=) \
    X(JUMPLT, <) \
    X(JUMPGT, >) \
    X(JUMPLEQ, <=) \
    X(JUMPGEQ, >=)

// X(name, operation)
// l and r are defined by client and have type vm_uint
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

// X(name, operation)
// l and r are defined by client and have type vm_int
#define VM_SIGNED_BINARY_OPCODES \
    X(SSUB, l - r) \
    X(SMUL, l * r) \
    /* Even though the right parameter is technically unsigned, it is UB if */ \
    /* it overflows the bitwidth, so it's ok to cast to signed here */ \
    X(ASHR, l >> r)

// X(name, operation)
// n is defined by cliient and has type vm_uint
#define VM_UNSIGNED_UNARY_OPCODES \
    X(INC, n + 1) \
    X(DEC, n - 1) \
    X(INV, ~n)

// X(name, numInlineArgs)
#define VM_SPECIAL_OPCODES \
    X(LOAD, 1) \
    X(STORE, 1) \
    X(LOAD_IMM, 1) \
    X(DEREF, 0) \
    X(ARRAYLOAD, 2) \
    X(ARRAYSTORE, 2) \
    X(PICK, 1) \
    X(DUP, 0) \
    X(SWAP, 0) \
    X(DROP, 0) \
    X(IO, 1) \
    X(HALT, 0) \
    X(JUMP, 1)

// clang-format off
// disable format because X macros fuck with it

// Note size of enum MUST fit into bitwidth of vm_uint - 1
// to fit PEEK bit.

// No enum items should be declared here directly, except for meta-items such
// as enum size. The assembler works only off of the X-macros defined above to
// generate opcode schema.
typedef enum
{

#define X(name, numInlineArgs) VM_OPCODE_##name,
    VM_SPECIAL_OPCODES
#undef X

#define X(name, op) VM_OPCODE_##name,
    VM_JUMP_OPCODES

    // Operations
    VM_UNSIGNED_BINARY_OPCODES
    VM_SIGNED_BINARY_OPCODES
    VM_UNSIGNED_UNARY_OPCODES
#undef X

    VM_NUM_OPCODES

} vm_opcode_t;

// clang-format on

#endif
