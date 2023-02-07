#ifndef VM_PLATFORM_INTERFACE_H
#define VM_PLATFORM_INTERFACE_H

#include <stdbool.h>
#include "basetypes.h"
#include "state.h"

typedef void (*vm_ioFunction_t)(
    vm_state_t * state, vm_uword_t * argsBuffer, vm_uword_t * returnPointer);

#define VM_DEFINE_IO_INTERFACE(name) \
    void name(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)

#define VM_IO_FN_MAX_ARGS 8

typedef struct
{
    vm_ioFunction_t callback;
    vm_uword_t numArgs;
    bool hasReturn;
} vm_ioFunctionRegistryItem_t;

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uword_t * outRegistryListLength);

#endif
