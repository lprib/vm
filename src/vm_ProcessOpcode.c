#include "vm_ProcessOpcode.h"

#include "vm_BaseTypes.h"
#include "vm_Io.h"
#include "vm_Opcodes.h"
#include "vm_State.h"

#include <stdbool.h>
#include <stdio.h>

#define POP_OR_PEEK(state, idx) \
    peek ? vm_PeekStack(state, idx) : vm_PopStack(state)

vm_programTickResult_t vm_ProcessNextOpcode(vm_state_t * s)
{
    vm_uint opcode = vm_GetMemAndIncrememt(s);

    bool peek = opcode & VM_PEEK_BITMASK;
    // clear peek bit before switching
    opcode &= ~VM_PEEK_BITMASK;

    vm_programTickResult_t result = VM_PROCESS_CONTINUE;

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
        vm_uint val = POP_OR_PEEK(s, 0);
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
        vm_uint a = POP_OR_PEEK(s, 0);
        vm_uint b = POP_OR_PEEK(s, 1);
        vm_PushStack(s, a);
        vm_PushStack(s, b);
    }
    break;

    case VM_OPCODE_JUMP:
    {
        s->pc = &s->mem[vm_GetMemAndIncrememt(s)];
    }
    break;

    case VM_OPCODE_IO:
    {
        vm_uint fnIndex = vm_GetMemAndIncrememt(s);
        if (!vm_IoFnCall(s, fnIndex, peek))
        {
            result = VM_PROCESS_ERROR_UNDEF_IO_FN;
        }
    }
    break;

    case VM_OPCODE_HALT:
    {
        result = VM_PROCESS_PROGRAM_HALT;
    }
    break;

#define X(name, op) \
    case VM_OPCODE_JUMP##name: \
    { \
        vm_uint r = POP_OR_PEEK(s, 0); \
        vm_uint l = POP_OR_PEEK(s, 1); \
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
        vm_uint r = POP_OR_PEEK(s, 0); \
        vm_uint l = POP_OR_PEEK(s, 1); \
        vm_PushStack(s, VM_TO_UNSIGNED(op)); \
    } \
    break;
        VM_OPCODE_BINARY_OPS
#undef X

#define X(name, op) \
    case VM_OPCODE_##name: \
    { \
        vm_uint n = POP_OR_PEEK(s, 0); \
        vm_PushStack(s, op); \
    } \
    break;
        VM_OPCODE_UNARY_OPS
#undef X
    }

    return result;
}
