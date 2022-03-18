#ifndef VM_STATE_H
#define VM_STATE_H

#include <stdint.h>

typedef struct
{
    int16_t pc;
    int16_t sp;
    int16_t * stack;
    int16_t stack_size;
    int16_t * mem;
    int16_t mem_size;
} vm_state_t;

void vm_InitState(
    vm_state_t * state,
    int16_t * stack,
    int stack_size,
    int16_t * mem,
    int mem_size);

#endif
