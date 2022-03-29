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

    state->pc = mem;
    // initially points off the stack since nothing has been pushed
    state->sp = stack + stack_size;
}

void vm_PushStack(vm_state_t * state, vm_uint val)
{
    state->sp--;
    *state->sp = val;
}

vm_uint vm_PopStack(vm_state_t * state)
{
    vm_uint ret = *state->sp;
    state->sp++;
    return ret;
}

vm_uint vm_PeekStack(vm_state_t * state, vm_uint index)
{
    return state->sp[index];
}

/** take value at index in stack, and shuffle rest into the gap */
vm_uint vm_TakeStack(vm_state_t * state, vm_uint index)
{
    vm_uint ret = state->sp[index];
    for(int i = index; i > 0; i--)
    {
        state->sp[i] = state->sp[i - 1];
    }
    state->sp++;
    return ret;
}

vm_uint vm_GetMemAndIncrement(vm_state_t * state)
{
    vm_uint ret = *state->pc;
    state->pc++;
    return ret;
}
