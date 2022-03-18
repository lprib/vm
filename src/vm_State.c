#include "vm_State.h"

#include <stdint.h>

/** Initialize VM state with memory and stack segments */
void vm_InitState(
    vm_state_t * state,
    int16_t * stack,
    int stack_size,
    int16_t * mem,
    int mem_size)
{
    state->stack = stack;
    state->stack_size = stack_size;

    state->mem = mem;
    state->mem_size = mem_size;

    state->pc = 0;
    // initially points off the stack since nothing has been pushed
    state->sp = stack_size;
}
