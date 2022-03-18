#include "vm_State.h"

#include <stdint.h>

/** Initialize VM state with memory and stack segments */
void vm_InitState(
    vm_state_t * state,
    vm_uint * stack,
    vm_uint stack_size,
    vm_uint * mem,
    vm_uint mem_size)
{
    state->stack = stack;
    state->stack_size = stack_size;

    state->mem = mem;
    state->mem_size = mem_size;

    state->pc = 0;
    // initially points off the stack since nothing has been pushed
    state->sp = stack_size;
}

void vm_PushStack(vm_state_t * state, vm_uint val)
{
    state->sp--;
    state->stack[state->sp] = val;
}

vm_uint vm_PopStack(vm_state_t * state)
{
    vm_uint ret = state->stack[state->sp];
    state->sp++;
    return ret;
}
