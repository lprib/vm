#include "basetypes.h"
#include "io.h"
#include "platforminterface.h"
#include "state.h"

bool io_fncall(vm_state_t * state, vm_uword_t fnIndex, bool peek)
{
    io_fn_spec_t * registryList;
    vm_uword_t registryListLength;

    // TODO optimization: init this at startup?
    interface_getiofns(&registryList, &registryListLength);

    // Check whether the fnIndex is valid
    if (fnIndex > registryListLength)
    {
        return false;
    }

    io_fn_spec_t * registryItem = &registryList[fnIndex];

    vm_uword_t argsBuffer[VM_IO_FN_MAX_ARGS] = {0};
    vm_uword_t returnValue = 0;

    // Fill args buffer in reverse since the oldest pushed value should be the
    // first arg
    int maxArgIndex = registryItem->numArgs - 1;
    for (int i = maxArgIndex; i >= 0; i--)
    {
        argsBuffer[i] =
            peek ? state_peekstack(state, maxArgIndex - i) : state_popstack(state);
    }

    registryItem->callback(state, argsBuffer, &returnValue);

    if (registryItem->hasReturn)
    {
        state_pushstack(state, returnValue);
    }

    return true;
}
