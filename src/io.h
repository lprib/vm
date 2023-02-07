#ifndef VM_IO_H
#define VM_IO_H

#include "basetypes.h"
#include "state.h"

#include <stdbool.h>

// returns whether the fnIndex is a valid index
bool vm_IoFnCall(vm_state_t * state, vm_uword_t fnIndex, bool peek);

#endif
