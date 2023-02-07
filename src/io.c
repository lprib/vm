#include "basetypes.h"
#include "io.h"
#include "state.h"

vm_tick_result_t io_fncall(vm_state_t * state, vm_uword_t fnIndex, bool peek)
{
    // Check whether the fnIndex is valid
    if (fnIndex > IO_MAX_NUM_FNS)
    {
        return VM_PROCESS_ERROR_UNDEF_IO_FN;
    }

    io_fn_spec_t* registryItem = &state->io_fns[fnIndex];
    
    if(!registryItem->callback)
    {
        return VM_PROCESS_ERROR_UNDEF_IO_FN;
    }

    vm_uword_t argsBuffer[IO_MAX_NUM_ARGS] = {0};
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

    return VM_PROCESS_CONTINUE;
}

void io_register(vm_state_t * state, vm_uword_t fn_index, io_fn_spec_t fn_spec)
{
    state->io_fns[fn_index] = fn_spec;
}