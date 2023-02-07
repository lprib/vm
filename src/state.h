#ifndef VM_STATE_H
#define VM_STATE_H

#include "basetypes.h"

#include <stdint.h>

typedef struct
{
    // Points inside of mem
    vm_uword_t * pc;
    // Points inside of stack
    vm_uword_t * sp;

    vm_uword_t * stack;
    vm_uword_t stack_size;
    vm_uword_t * mem;
    vm_uword_t mem_size;
} vm_state_t;

void state_init(
    vm_state_t * state,
    vm_uword_t * stack,
    vm_uword_t stack_size,
    vm_uword_t * mem,
    vm_uword_t mem_size);

void state_pushstack(vm_state_t * state, vm_uword_t val);

vm_uword_t state_popstack(vm_state_t * state);

vm_uword_t state_peekstack(vm_state_t * state, vm_uword_t index);

vm_uword_t state_takestack(vm_state_t * state, vm_uword_t index);

vm_uword_t state_nextinstr(vm_state_t * state);

vm_uword_t state_getmem(vm_state_t* state, vm_uword_t addr);

void state_setmem(vm_state_t* state, vm_uword_t addr, vm_uword_t val);

#endif
