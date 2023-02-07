#include "state.h"

#include <string.h>
#include <stdint.h>

/** Initialize VM state with memory and stack segments */
void state_init(
    vm_state_t * state,
    vm_uword_t * stack,
    vm_uword_t stack_size,
    vm_uword_t * mem,
    vm_uword_t mem_size)
{
    state->stack = stack;
    state->stack_size = stack_size;

    state->mem = mem;
    state->mem_size = mem_size;

    state->pc = mem;
    // initially points off the stack since nothing has been pushed
    state->sp = stack + stack_size;
    
    memset(&state->io_fns, 0, sizeof(state->io_fns));
}

void state_pushstack(vm_state_t * state, vm_uword_t val)
{
    state->sp--;
    *state->sp = val;
}

vm_uword_t state_popstack(vm_state_t * state)
{
    vm_uword_t ret = *state->sp;
    state->sp++;
    return ret;
}

vm_uword_t state_peekstack(vm_state_t * state, vm_uword_t index)
{
    return state->sp[index];
}

/** take value at index in stack, and shuffle rest into the gap */
vm_uword_t state_takestack(vm_state_t * state, vm_uword_t index)
{
    vm_uword_t ret = state->sp[index];
    for(int i = index; i > 0; i--)
    {
        state->sp[i] = state->sp[i - 1];
    }
    state->sp++;
    return ret;
}

vm_uword_t state_nextinstr(vm_state_t * state)
{
    vm_uword_t ret = *state->pc;
    state->pc++;
    return ret;
}

// TODO inline
vm_uword_t state_getmem(vm_state_t* state, vm_uword_t addr)
{
    return state->mem[addr];
}

void state_setmem(vm_state_t* state, vm_uword_t addr, vm_uword_t val)
{
    state->mem[addr] = val;
}

vm_uword_t state_getstackusage(vm_state_t * state)
{
    return (state->stack + state->stack_size) - state->sp;
}