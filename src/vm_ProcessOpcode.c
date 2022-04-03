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
    vm_uint raw_opcode = vm_GetProgramAndIncrement(s);
    bool peek = raw_opcode & VM_PEEK_BITMASK;
    // clear peek bit before switching
    vm_opcode_t opcode = (vm_opcode_t)(raw_opcode & ~VM_PEEK_BITMASK);

    vm_programTickResult_t result = VM_PROCESS_CONTINUE;

    switch (opcode)
    {

    case VM_OP_LOAD:
    {
        vm_uint addr = vm_GetProgramAndIncrement(s);
        vm_PushStack(s, s->mem[addr]);
    }
    break;

    case VM_OP_STORE:
    {
        vm_uint addr = vm_GetProgramAndIncrement(s);
        vm_uint val = POP_OR_PEEK(s, 0);
        s->mem[addr] = val;
    }
    break;

    case VM_OP_LOADIMM:
    {
        vm_PushStack(s, vm_GetProgramAndIncrement(s));
    }
    break;

    case VM_OP_DEREF:
    {
        vm_uint addr = POP_OR_PEEK(s, 0);
        vm_PushStack(s, vm_GetMem(s, addr));
    }
    break;

    case VM_OP_ARRAYLOAD:
    {
        vm_uint base = vm_GetProgramAndIncrement(s);
        vm_uint offset_and_size = vm_GetProgramAndIncrement(s);

        vm_uint offset = offset_and_size >> VM_HIGH_PART_SHIFT;
        vm_uint size = offset_and_size & VM_LOW_PART_MASK;
        vm_uint index = POP_OR_PEEK(s, 0);
        vm_PushStack(s, vm_GetMem(s, base + index * size + offset));
    }
    break;

    case VM_OP_ARRAYSTORE:
    {
        vm_uint base = vm_GetProgramAndIncrement(s);
        vm_uint offset_and_size = vm_GetProgramAndIncrement(s);

        vm_uint offset = offset_and_size >> VM_HIGH_PART_SHIFT;
        vm_uint size = offset_and_size & VM_LOW_PART_MASK;
        vm_uint index = POP_OR_PEEK(s, 0);
        vm_uint val = POP_OR_PEEK(s, 1);

        vm_SetMem(s, base + index * size + offset, val);
    }
    break;

    case VM_OP_PICK:
    {
        vm_uint stackIndex = vm_GetProgramAndIncrement(s);
        if (peek)
        {
            vm_PushStack(s, vm_PeekStack(s, stackIndex));
        }
        else
        {
            vm_PushStack(s, vm_TakeStack(s, stackIndex));
        }
    }
    break;

    case VM_OP_DUP:
    {
        vm_PushStack(s, vm_PeekStack(s, 0));
    }
    break;

    case VM_OP_SWAP:
    {
        vm_uint a = POP_OR_PEEK(s, 0);
        vm_uint b = POP_OR_PEEK(s, 1);
        vm_PushStack(s, a);
        vm_PushStack(s, b);
    }
    break;

    case VM_OP_DROP:
    {
        (void)vm_PopStack(s);
    }
    break;

    case VM_OP_JUMP:
    {
        s->pc = &s->mem[vm_GetProgramAndIncrement(s)];
    }
    break;

    case VM_OP_IO:
    {
        vm_uint fnIndex = vm_GetProgramAndIncrement(s);
        if (!vm_IoFnCall(s, fnIndex, peek))
        {
            result = VM_PROCESS_ERROR_UNDEF_IO_FN;
        }
    }
    break;

    case VM_OP_HALT:
    {
        result = VM_PROCESS_PROGRAM_HALT;
    }
    break;

#define X(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uint r = POP_OR_PEEK(s, 0); \
        vm_uint l = POP_OR_PEEK(s, 1); \
        vm_uint jumpDestination = vm_GetProgramAndIncrement(s); \
        if (l op r) \
        { \
            s->pc = &s->mem[jumpDestination]; \
        } \
    } \
    break;

        VM_JUMP_OPCODES
#undef X

#define X(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uint r = POP_OR_PEEK(s, 0); \
        vm_uint l = POP_OR_PEEK(s, 1); \
        vm_uint res = op; \
        vm_PushStack(s, res); \
    } \
    break;
        VM_UNSIGNED_BINARY_OPCODES
#undef X

#define X(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uint r_unsigned = POP_OR_PEEK(s, 0); \
        vm_uint l_unsigned = POP_OR_PEEK(s, 1); \
        vm_int r = ((union { \
                       vm_int i; \
                       vm_uint u; \
                   }){.u = r_unsigned}) \
                       .i; \
        vm_int l = ((union { \
                       vm_int i; \
                       vm_uint u; \
                   }){.u = l_unsigned}) \
                       .i; \
        vm_int res_signed = op; \
        vm_int res = ((union { \
                         vm_int i; \
                         vm_uint u; \
                     }){.i = res_signed}) \
                         .u; \
        vm_PushStack(s, res); \
    } \
    break;
        VM_SIGNED_BINARY_OPCODES
#undef X

#define X(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uint n = POP_OR_PEEK(s, 0); \
        vm_PushStack(s, op); \
    } \
    break;
        VM_UNSIGNED_UNARY_OPCODES
#undef X

    // Have to do this for exhaustive handling
    case VM_NUM_OPCODES:
        break;
    }

    return result;
}
