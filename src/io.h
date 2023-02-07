#ifndef VM_IO_H
#define VM_IO_H

#include "basetypes.h"
#include "state.h"

#include <stdbool.h>

// returns whether the fnIndex is a valid index
bool io_fncall(vm_state_t * state, vm_uword_t fnIndex, bool peek);

#endif
