#ifndef VM_PLATFORM_INTERFACE_H
#define VM_PLATFORM_INTERFACE_H

#include "vm_BaseTypes.h"

typedef void (*vm_ioFunction_t)(vm_uint * argsBuffer, vm_uint * returnPointer);

#define VM_DEFINE_IO_INTERFACE(name) \
    void name(vm_uint * args, vm_uint * outReturn)

#define VM_IO_FN_MAX_ARGS 8

typedef struct
{
    vm_ioFunction_t callback;
    vm_uint numArgs;
    vm_bool hasReturn;
} vm_ioFunctionRegistryItem_t;

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uint * outRegistryListLength);

#endif