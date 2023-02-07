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

void vm_InitState(
    vm_state_t * state,
    vm_uword_t * stack,
    vm_uword_t stack_size,
    vm_uword_t * mem,
    vm_uword_t mem_size);

void vm_PushStack(vm_state_t * state, vm_uword_t val);

vm_uword_t vm_PopStack(vm_state_t * state);

vm_uword_t vm_PeekStack(vm_state_t * state, vm_uword_t index);

vm_uword_t vm_TakeStack(vm_state_t * state, vm_uword_t index);

vm_uword_t vm_GetProgramAndIncrement(vm_state_t * state);

vm_uword_t vm_GetMem(vm_state_t* state, vm_uword_t addr);

void vm_SetMem(vm_state_t* state, vm_uword_t addr, vm_uword_t val);

#endif
