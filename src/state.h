#ifndef VM_STATE_H
#define VM_STATE_H

#include "basetypes.h"

#include <stdbool.h>
#include <stdint.h>

#define IO_MAX_NUM_FNS 32
#define IO_MAX_NUM_ARGS 8

struct vm_state_s;

typedef void (*io_fn_t)(
    struct vm_state_s * state, vm_uword_t * argsBuffer, vm_uword_t * returnPointer);

typedef struct
{
    io_fn_t callback;
    vm_uword_t numArgs;
    bool hasReturn;
} io_fn_spec_t;

typedef struct vm_state_s
{
    // Points inside of mem
    vm_uword_t * pc;
    // Points inside of stack
    vm_uword_t * sp;

    vm_uword_t * stack;
    vm_uword_t stack_size;
    vm_uword_t * mem;
    vm_uword_t mem_size;
    
    io_fn_spec_t io_fns[IO_MAX_NUM_FNS];
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

vm_uword_t state_getstackusage(vm_state_t * state);

#endif
