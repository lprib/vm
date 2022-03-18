#include "vm_ProcessOpcode.h"

#include "vm_BaseTypes.h"
#include "vm_Opcodes.h"
#include "vm_State.h"

#include <stdio.h>

void vm_ProcessNextOpcode(vm_state_t * s)
{
    vm_uint opcode = vm_GetMemAndIncrememt(s);

    switch (opcode)
    {

    case VM_OPCODE_LOAD:
    {
        vm_uint addr = vm_GetMemAndIncrememt(s);
        vm_PushStack(s, s->mem[addr]);
    }
    break;

    case VM_OPCODE_STORE:
    {
        vm_uint addr = vm_GetMemAndIncrememt(s);
        vm_uint val = vm_PopStack(s);
        s->mem[addr] = val;
    }
    break;

    case VM_OPCODE_LOAD_IMM:
    {
        vm_PushStack(s, vm_GetMemAndIncrememt(s));
    }
    break;

    case VM_OPCODE_DUP:
    {
        vm_PushStack(s, vm_PeekStack(s, 0));
    }
    break;

    case VM_OPCODE_SWAP:
    {
        vm_uint a = vm_PopStack(s);
        vm_uint b = vm_PopStack(s);
        vm_PushStack(s, a);
        vm_PushStack(s, b);
    }
    break;

    case VM_OPCODE_JUMP:
    {
        s->pc = &s->mem[vm_GetMemAndIncrememt(s)];
    }
    break;

#define X(name, op) \
    case VM_OPCODE_JUMP##name: \
    { \
        vm_uint r = vm_PopStack(s); \
        vm_uint l = vm_PopStack(s); \
        vm_uint jumpDestination = vm_GetMemAndIncrememt(s); \
        if (l op r) \
        { \
            s->pc = &s->mem[jumpDestination]; \
        } \
    } \
    break;

        VM_OPCODE_JUMPS
#undef X

#define X(name, op) \
    case VM_OPCODE_##name: \
    { \
        vm_uint r = vm_PopStack(s); \
        vm_uint l = vm_PopStack(s); \
        vm_PushStack(s, VM_TO_UNSIGNED(op)); \
    } \
    break;
        VM_OPCODE_BINARY_OPS
#undef X

#define X(name, op) \
    case VM_OPCODE_##name: \
    { \
        vm_uint n = vm_PopStack(s); \
        vm_PushStack(s, op); \
    } \
    break;
        VM_OPCODE_UNARY_OPS
#undef X
    }
}
