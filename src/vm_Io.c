#include "vm_Io.h"

#include "vm_BaseTypes.h"
#include "vm_PlatormInterface.h"
#include "vm_State.h"

bool vm_IoFnCall(vm_state_t * state, vm_uint fnIndex, bool peek)
{
    vm_ioFunctionRegistryItem_t * registryList;
    vm_uint registryListLength;

    // TODO optimization: init this at startup?
    vmint_GetIoFunctionRegistry(&registryList, &registryListLength);

    // Check whether the fnIndex is valid
    if (fnIndex > registryListLength)
    {
        return false;
    }

    vm_ioFunctionRegistryItem_t * registryItem = &registryList[fnIndex];

    vm_uint argsBuffer[VM_IO_FN_MAX_ARGS] = {0};
    vm_uint returnValue = 0;

    // Fill args buffer in reverse since the oldest pushed value should be the
    // first arg
    int maxArgIndex = registryItem->numArgs - 1;
    for (int i = maxArgIndex; i >= 0; i--)
    {
        argsBuffer[i] =
            peek ? vm_PeekStack(state, maxArgIndex - i) : vm_PopStack(state);
    }

    registryItem->callback(state, argsBuffer, &returnValue);

    if (registryItem->hasReturn)
    {
        vm_PushStack(state, returnValue);
    }

    return true;
}
