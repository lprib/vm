#include "interpreter.h"

#include "basetypes.h"
#include "io.h"
#include "opcodes.h"
#include "state.h"

#include <stdbool.h>
#include <stdio.h>

#define POP_OR_PEEK(state, idx) \
    peek ? state_peekstack(state, idx) : state_popstack(state)

vm_tick_result_t interpret_next_op(vm_state_t * s)
{
    vm_uword_t raw_opcode = state_nextinstr(s);
    bool peek = raw_opcode & VM_PEEK_BITMASK;
    // clear peek bit before switching
    vm_opcode_t opcode = (vm_opcode_t)(raw_opcode & ~VM_PEEK_BITMASK);

    vm_tick_result_t result = VM_PROCESS_CONTINUE;

    switch (opcode)
    {

    case VM_OP_LOAD:
    {
        vm_uword_t addr = state_nextinstr(s);
        state_pushstack(s, s->mem[addr]);
    }
    break;

    case VM_OP_STORE:
    {
        vm_uword_t addr = state_nextinstr(s);
        vm_uword_t val = POP_OR_PEEK(s, 0);
        s->mem[addr] = val;
    }
    break;

    case VM_OP_LOADIMM:
    {
        state_pushstack(s, state_nextinstr(s));
    }
    break;

    case VM_OP_DEREF:
    {
        vm_uword_t addr = POP_OR_PEEK(s, 0);
        state_pushstack(s, state_getmem(s, addr));
    }
    break;

    case VM_OP_ARRAYLOAD:
    {
        vm_uword_t base = state_nextinstr(s);
        vm_uword_t offset = state_nextinstr(s);
        vm_uword_t size = state_nextinstr(s);

        vm_uword_t index = POP_OR_PEEK(s, 0);
        state_pushstack(s, state_getmem(s, base + index * size + offset));
    }
    break;

    case VM_OP_ARRAYSTORE:
    {
        vm_uword_t base = state_nextinstr(s);
        vm_uword_t offset = state_nextinstr(s);
        vm_uword_t size = state_nextinstr(s);

        vm_uword_t index = POP_OR_PEEK(s, 0);
        vm_uword_t val = POP_OR_PEEK(s, 1);

        state_setmem(s, base + index * size + offset, val);
    }
    break;

    case VM_OP_PICK:
    {
        vm_uword_t stackIndex = state_nextinstr(s);
        if (peek)
        {
            state_pushstack(s, state_peekstack(s, stackIndex));
        }
        else
        {
            state_pushstack(s, state_takestack(s, stackIndex));
        }
    }
    break;

    case VM_OP_DUP:
    {
        state_pushstack(s, state_peekstack(s, 0));
    }
    break;

    case VM_OP_SWAP:
    {
        vm_uword_t a = POP_OR_PEEK(s, 0);
        vm_uword_t b = POP_OR_PEEK(s, 1);
        state_pushstack(s, a);
        state_pushstack(s, b);
    }
    break;

    case VM_OP_DROP:
    {
        (void)state_popstack(s);
    }
    break;

    case VM_OP_JUMP:
    {
        s->pc = &s->mem[state_nextinstr(s)];
    }
    break;

    case VM_OP_IO:
    {
        vm_uword_t fnIndex = state_nextinstr(s);
        if (!io_fncall(s, fnIndex, peek))
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
        vm_uword_t jumpDestination = state_nextinstr(s); \
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
        state_pushstack(s, res); \
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
        state_pushstack(s, res); \
    } \
    break;

        VM_SIGNED_BINARY_OPCODES(GEN_SIGNED_BINARY_OPCODE_CASE)

#define GEN_UNSIGNED_UNARY_OPCODE_CASE(name, _numInlineArgs, op) \
    case VM_OP_##name: \
    { \
        vm_uword_t n = POP_OR_PEEK(s, 0); \
        state_pushstack(s, op); \
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
