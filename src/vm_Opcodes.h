#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#include "vm_BaseTypes.h"

// store the peek flag in the high bits of opcode
#define VM_PEEK_BITMASK (1 << (VM_INT_BITWIDTH - 1))

// X(name, numInlineArgs, _tokens<unused>)
#define VM_SPECIAL_OPCODES \
    X(LOAD, 1, ) \
    X(STORE, 1, ) \
    X(LOADIMM, 1, ) \
    X(DEREF, 0, ) \
    X(ARRAYLOAD, 2, ) \
    X(ARRAYSTORE, 2, ) \
    X(PICK, 1, ) \
    X(DUP, 0, ) \
    X(SWAP, 0, ) \
    X(DROP, 0, ) \
    X(IO, 1, ) \
    X(HALT, 0, ) \
    X(JUMP, 1, )

// X(name, numInlineArgs, comparisionOperation)
#define VM_JUMP_OPCODES \
    X(JUMPEQ, 1, ==) \
    X(JUMPNEQ, 1, !=) \
    X(JUMPLT, 1, <) \
    X(JUMPGT, 1, >) \
    X(JUMPLEQ, 1, <=) \
    X(JUMPGEQ, 1, >=)

// X(name, numInlineArgs, operation)
// l and r are defined by client and have type vm_uint
#define VM_UNSIGNED_BINARY_OPCODES \
    X(ADD, 0, l + r) \
    X(USUB, 0, l - r) \
    X(UMUL, 0, l * r) \
    X(DIV, 0, l / r) \
    X(AND, 0, l & r) \
    X(OR, 0, l | r) \
    X(XOR, 0, l ^ r) \
    X(MOD, 0, l % r) \
    X(SHL, 0, l << r) \
    X(LSHR, 0, l >> r)

// X(name, numInlineArgs, operation)
// l and r are defined by client and have type vm_int
#define VM_SIGNED_BINARY_OPCODES \
    X(SSUB, 0, l - r) \
    X(SMUL, 0, l * r) \
    /* Even though the right parameter is technically unsigned, it is UB if */ \
    /* it overflows the bitwidth, so it's ok to cast to signed here */ \
    X(ASHR, 0, l >> r)

// X(name, numInlineArgs, operation)
// n is defined by cliient and has type vm_uint
#define VM_UNSIGNED_UNARY_OPCODES \
    X(INC, 0, n + 1) \
    X(DEC, 0, n - 1) \
    X(INV, 0, ~n)

// Each x-macros list here must comform to X(name, numInlineArgs, tokens).
// tokens can be anything, and is used by vm_ProcessOpcodes.c to perform
// operations per opcode
#define VM_ALL_OPCODES \
    VM_SPECIAL_OPCODES \
    VM_JUMP_OPCODES \
    VM_UNSIGNED_BINARY_OPCODES \
    VM_SIGNED_BINARY_OPCODES \
    VM_UNSIGNED_UNARY_OPCODES

// clang-format off
// disable format because X macros fuck with it

// Note size of enum MUST fit into bitwidth of vm_uint - 1
// to fit PEEK bit.

// No enum items should be declared here directly, except for meta-items such
// as enum size. All operations must be defined as x-macros, and then added to
// the VM_ALL_OPCODES list. This list is also included in the opcode schema
// generation, for use by the assembler.
typedef enum
{

#define X(name, _numInlineArgs, _tokens) VM_OP_##name,
    VM_ALL_OPCODES
#undef X

    VM_NUM_OPCODES

} vm_opcode_t;

// clang-format on

#endif
