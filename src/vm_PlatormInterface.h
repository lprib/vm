#ifndef VM_PLATFORM_INTERFACE_H
#define VM_PLATFORM_INTERFACE_H

#include "vm_BaseTypes.h"

typedef void vm_ioFunction_t(vm_uint * argsBuffer, vm_uint * returnPointer);

#define VM_DEFINE_IO_INTERFACE(name) \
    void name(vm_uint * args, vm_uint * outReturn)

/**
 * Interfacing with VM:
 *
 * Provide vmint_IoFnRegistry.h:
 *
 * #define VM_IO_FN_REGISTRY \
 *     X(fn_name, num_args, has_return)
 *
 * #define VM_IO_FN_REGISTRY \
 *     X(ioFn1, 0, false), \
 *     X(ioFn2, 2, true)
 *
 * To define the functions, use:
 *
 * VM_DEFINE_IO_INTERFACE(ioFn1)
 * {
 *     vm_uint arg0 = args[0];
 *     *outReturn = arg0 + 1;
 * }
 *
 * VM_DEFINE_IO_INTERFACE(ioFn2)
 * {
 *     ...
 * }
 */

#endif
