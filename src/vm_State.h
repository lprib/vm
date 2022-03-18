#ifndef VM_STATE_H
#define VM_STATE_H

#include "vm_BaseTypes.h"

typedef struct
{
    vm_uint pc;
    vm_uint sp;
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

#endif
