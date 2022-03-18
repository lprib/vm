#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#define VM_SIGNED

#define VM_OPCODE_JUMPS \
    X(EQ, ==) \
    X(NEQ, !=) \
    X(LT, <) \
    X(GT, >) \
    X(LEQ, <=) \
    X(GEQ, >=)

#define VM_OPCODE_ARITH \
    X(ADD, l + r) \
    X(USUB, l - r) \
    X(SSUB, VM_TO_SIGNED(l) - VM_TO_SIGNED(r)) \
    X(UMUL, l * r) \
    X(SMUL, VM_TO_SIGNED(l) * VM_TO_SIGNED(r)) \
    X(DIV, l / r)

typedef enum
{
    VM_OPCODE_LOAD,
    VM_OPCODE_STORE,
    VM_OPCODE_JUMP,
#define X(name, op) VM_OPCODE_JUMP##name,
    VM_OPCODE_JUMPS
#undef X

#define X(name, op) VP_OPCODE_##name,
        VM_OPCODE_ARITH
#undef X

} vm_opcode_t;

#endif
