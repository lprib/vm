#include "vm_BaseTypes.h"
#include "vm_PlatormInterface.h"
#include "vm_ProcessOpcode.h"
#include "vm_State.h"

#include <stdio.h>

VM_DEFINE_IO_INTERFACE(it_Return1) { *outReturn = 100; }

VM_DEFINE_IO_INTERFACE(it_Arg1Return1)
{
    vm_uint arg = args[0];
    *outReturn = arg * 3;
}

VM_DEFINE_IO_INTERFACE(it_Arg3Return0)
{
    *outReturn = args[0] + args[1] + args[2];
}

VM_DEFINE_IO_INTERFACE(it_Print)
{
    vm_uint arg = args[0];
    printf("%d\n", arg);
}

VM_DEFINE_IO_INTERFACE(it_Print2)
{
    vm_uint arg0 = args[0];
    vm_uint arg1 = args[1];
    printf("%d %d\n", arg0, arg1);
}

vm_ioFunctionRegistryItem_t fnRegistry[] = {
    {&it_Return1, 0, true},
    {&it_Arg1Return1, 1, true},
    {&it_Arg3Return0, 3, true},
    {&it_Print, 1, false},
    {&it_Print2, 2, false},
};

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uint * outRegistryListLength)
{
    *outRegistryList = fnRegistry;
    *outRegistryListLength = sizeof(fnRegistry) / sizeof(fnRegistry[0]);
}

vm_uint const memSize = 100;
vm_uint const stackSize = 100;

vm_uint mem[memSize] = {
    // todo fill with io interface tests
};
vm_uint stack[stackSize] = {0};

int main(void) {}