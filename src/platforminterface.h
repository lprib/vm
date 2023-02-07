#ifndef VM_PLATFORM_INTERFACE_H
#define VM_PLATFORM_INTERFACE_H

#include "basetypes.h"
#include "state.h"

#include <stdbool.h>

typedef void (*io_fn_t)(
    vm_state_t * state, vm_uword_t * argsBuffer, vm_uword_t * returnPointer);

#define VM_DEFINE_IO_INTERFACE(name) \
    void name(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)

#define VM_IO_FN_MAX_ARGS 8

typedef struct
{
    io_fn_t callback;
    vm_uword_t numArgs;
    bool hasReturn;
} io_fn_spec_t;

void interface_getiofns(
    io_fn_spec_t ** outRegistryList,
    vm_uword_t * outRegistryListLength);

#endif
