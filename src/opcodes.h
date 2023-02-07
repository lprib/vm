#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#include "basetypes.h"

// clang-format off

// store the peek flag in the high bits of opcode
#define VM_PEEK_BITMASK (1 << (VM_WORD_BITWIDTH - 1))

// X(name, numInlineArgs, _tokens<unused>)
#define VM_SPECIAL_OPCODES(x) \
    x(LOAD, 1, ) \
    x(STORE, 1, ) \
    x(LOADIMM, 1, ) \
    x(DEREF, 0, ) \
    x(ARRAYLOAD, 2, ) \
    x(ARRAYSTORE, 2, ) \
    x(PICK, 1, ) \
    x(DUP, 0, ) \
    x(SWAP, 0, ) \
    x(DROP, 0, ) \
    x(IO, 1, ) \
    x(HALT, 0, ) \
    x(JUMP, 1, )

// X(name, numInlineArgs, comparisionOperation)
#define VM_JUMP_OPCODES(x) \
    x(JUMPEQ, 1, ==) \
    x(JUMPNEQ, 1, !=) \
    x(JUMPLT, 1, <) \
    x(JUMPGT, 1, >) \
    x(JUMPLEQ, 1, <=) \
    x(JUMPGEQ, 1, >=)

// X(name, numInlineArgs, operation)
// l and r are defined by client and have type vm_uint
#define VM_UNSIGNED_BINARY_OPCODES(x) \
    x(ADD, 0, l + r) \
    x(USUB, 0, l - r) \
    x(UMUL, 0, l * r) \
    x(DIV, 0, l / r) \
    x(AND, 0, l & r) \
    x(OR, 0, l | r) \
    x(XOR, 0, l ^ r) \
    x(MOD, 0, l % r) \
    x(SHL, 0, l << r) \
    x(LSHR, 0, l >> r)

// X(name, numInlineArgs, operation)
// l and r are defined by client and have type vm_int
#define VM_SIGNED_BINARY_OPCODES(x) \
    x(SSUB, 0, l - r) \
    x(SMUL, 0, l * r) \
    /* Even though the right parameter is technically unsigned, it is UB if */ \
    /* it overflows the bitwidth, so it's ok to cast to signed here */ \
    x(ASHR, 0, l >> r)

// X(name, numInlineArgs, operation)
// n is defined by cliient and has type vm_uint
#define VM_UNSIGNED_UNARY_OPCODES(x) \
    x(INC, 0, n + 1) \
    x(DEC, 0, n - 1) \
    x(INV, 0, ~n)

// Each x-macros list here must comform to X(name, numInlineArgs, tokens).
// tokens can be anything, and is used by vm_ProcessOpcodes.c to perform
// operations per opcode
#define VM_ALL_OPCODES(x) \
    VM_SPECIAL_OPCODES(x) \
    VM_JUMP_OPCODES(x) \
    VM_UNSIGNED_BINARY_OPCODES(x) \
    VM_SIGNED_BINARY_OPCODES(x) \
    VM_UNSIGNED_UNARY_OPCODES(x)

// disable format because X macros fuck with it

// Note size of enum MUST fit into bitwidth of vm_uint - 1
// to fit PEEK bit.

#define GEN_OPCODE_ENUM_ITEM(name, _numInlineArgs, _tokens) VM_OP_##name,
// No enum items should be declared here directly, except for meta-items such
// as enum size. All operations must be defined as x-macros, and then added to
// the VM_ALL_OPCODES list. This list is also included in the opcode schema
// generation, for use by the assembler.
typedef enum
{
    VM_ALL_OPCODES(GEN_OPCODE_ENUM_ITEM)
    VM_NUM_OPCODES
} vm_opcode_t;

// clang-format on

#endif
