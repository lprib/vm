#include "basetypes.h"
#include "io.h"
#include "platforminterface.h"
#include "state.h"

bool vm_IoFnCall(vm_state_t * state, vm_uword_t fnIndex, bool peek)
{
    vm_ioFunctionRegistryItem_t * registryList;
    vm_uword_t registryListLength;

    // TODO optimization: init this at startup?
    vmint_GetIoFunctionRegistry(&registryList, &registryListLength);

    // Check whether the fnIndex is valid
    if (fnIndex > registryListLength)
    {
        return false;
    }

    vm_ioFunctionRegistryItem_t * registryItem = &registryList[fnIndex];

    vm_uword_t argsBuffer[VM_IO_FN_MAX_ARGS] = {0};
    vm_uword_t returnValue = 0;

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
