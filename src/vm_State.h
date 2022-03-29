#ifndef VM_STATE_H
#define VM_STATE_H

#include "vm_BaseTypes.h"

#include <stdint.h>

typedef struct
{
    // Points inside of mem
    vm_uint * pc;
    // Points inside of stack
    vm_uint * sp;

    vm_uint * stack;
    vm_uint stack_size;
    vm_uint * mem;
    vm_uint mem_size;
} vm_state_t;

void vm_InitState(
    vm_state_t * state,
    vm_uint * stack,
    vm_uint stack_size,
    vm_uint * mem,
    vm_uint mem_size);

void vm_PushStack(vm_state_t * state, vm_uint val);

vm_uint vm_PopStack(vm_state_t * state);

vm_uint vm_PeekStack(vm_state_t * state, vm_uint index);

vm_uint vm_TakeStack(vm_state_t * state, vm_uint index);

vm_uint vm_GetMemAndIncrement(vm_state_t * state);

#endif
