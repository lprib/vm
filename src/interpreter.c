#include "interpreter.h"

#include "basetypes.h"
#include "io.h"
#include "opcodes.h"
#include "state.h"

#include <stdbool.h>
#include <stdio.h>

#define POP_OR_PEEK(state, idx) \
    peek ? vm_PeekStack(state, idx) : vm_PopStack(state)

vm_programTickResult_t vm_ProcessNextOpcode(vm_state_t * s)
{
    vm_uword_t raw_opcode = vm_GetProgramAndIncrement(s);
    bool peek = raw_opcode & VM_PEEK_BITMASK;
    // clear peek bit before switching
    vm_opcode_t opcode = (vm_opcode_t)(raw_opcode & ~VM_PEEK_BITMASK);

    vm_programTickResult_t result = VM_PROCESS_CONTINUE;

    switch (opcode)
    {

    case VM_OP_LOAD:
    {
        vm_uword_t addr = vm_GetProgramAndIncrement(s);
        vm_PushStack(s, s->mem[addr]);
    }
    break;

    case VM_OP_STORE:
    {
        vm_uword_t addr = vm_GetProgramAndIncrement(s);
        vm_uword_t val = POP_OR_PEEK(s, 0);
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
        vm_uword_t addr = POP_OR_PEEK(s, 0);
        vm_PushStack(s, vm_GetMem(s, addr));
    }
    break;

    case VM_OP_ARRAYLOAD:
    {
        vm_uword_t base = vm_GetProgramAndIncrement(s);
        vm_uword_t offset = vm_GetProgramAndIncrement(s);
        vm_uword_t size = vm_GetProgramAndIncrement(s);

        vm_uword_t index = POP_OR_PEEK(s, 0);
        vm_PushStack(s, vm_GetMem(s, base + index * size + offset));
    }
    break;

    case VM_OP_ARRAYSTORE:
    {
        vm_uword_t base = vm_GetProgramAndIncrement(s);
        vm_uword_t offset = vm_GetProgramAndIncrement(s);
        vm_uword_t size = vm_GetProgramAndIncrement(s);

        vm_uword_t index = POP_OR_PEEK(s, 0);
        vm_uword_t val = POP_OR_PEEK(s, 1);

        vm_SetMem(s, base + index * size + offset, val);
    }
    break;

    case VM_OP_PICK:
    {
        vm_uword_t stackIndex = vm_GetProgramAndIncrement(s);
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
        vm_uword_t a = POP_OR_PEEK(s, 0);
        vm_uword_t b = POP_OR_PEEK(s, 1);
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
        vm_uword_t fnIndex = vm_GetProgramAndIncrement(s);
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

#define GEN_JUMP_OPCODE_CASE(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uword_t r = POP_OR_PEEK(s, 0); \
        vm_uword_t l = POP_OR_PEEK(s, 1); \
        vm_uword_t jumpDestination = vm_GetProgramAndIncrement(s); \
        if (l op r) \
        { \
            s->pc = &s->mem[jumpDestination]; \
        } \
    } \
    break;

        VM_JUMP_OPCODES(GEN_JUMP_OPCODE_CASE)

#define GEN_UNSIGNED_BINARY_OPCODE_CASE(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uword_t r = POP_OR_PEEK(s, 0); \
        vm_uword_t l = POP_OR_PEEK(s, 1); \
        vm_uword_t res = op; \
        vm_PushStack(s, res); \
    } \
    break;

        VM_UNSIGNED_BINARY_OPCODES(GEN_UNSIGNED_BINARY_OPCODE_CASE)

#define GEN_SIGNED_BINARY_OPCODE_CASE(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uword_t r_unsigned = POP_OR_PEEK(s, 0); \
        vm_uword_t l_unsigned = POP_OR_PEEK(s, 1); \
        vm_sword_t r = ((union { \
                       vm_sword_t i; \
                       vm_uword_t u; \
                   }){.u = r_unsigned}) \
                       .i; \
        vm_sword_t l = ((union { \
                       vm_sword_t i; \
                       vm_uword_t u; \
                   }){.u = l_unsigned}) \
                       .i; \
        vm_sword_t res_signed = op; \
        vm_sword_t res = ((union { \
                         vm_sword_t i; \
                         vm_uword_t u; \
                     }){.i = res_signed}) \
                         .u; \
        vm_PushStack(s, res); \
    } \
    break;

        VM_SIGNED_BINARY_OPCODES(GEN_SIGNED_BINARY_OPCODE_CASE)

#define GEN_UNSIGNED_UNARY_OPCODE_CASE(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uword_t n = POP_OR_PEEK(s, 0); \
        vm_PushStack(s, op); \
    } \
    break;

        VM_UNSIGNED_UNARY_OPCODES(GEN_UNSIGNED_UNARY_OPCODE_CASE)

    default:
    {
        result = VM_PROCESS_ERROR_INVALID_OPCODE;
    }
    break;
    }

    return result;
}
