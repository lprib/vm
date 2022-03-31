#ifndef VM_PLATFORM_INTERFACE_H
#define VM_PLATFORM_INTERFACE_H

#include <stdbool.h>
#include "vm_BaseTypes.h"
#include "vm_State.h"

typedef void (*vm_ioFunction_t)(
    vm_state_t * state, vm_uint * argsBuffer, vm_uint * returnPointer);

#define VM_DEFINE_IO_INTERFACE(name) \
    void name(vm_state_t * state, vm_uint * args, vm_uint * outReturn)

#define VM_IO_FN_MAX_ARGS 8

typedef struct
{
    vm_ioFunction_t callback;
    vm_uint numArgs;
    bool hasReturn;
} vm_ioFunctionRegistryItem_t;

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uint * outRegistryListLength);

#endif
