#ifndef VM_IO_H
#define VM_IO_H

#include "vm_BaseTypes.h"
#include "vm_State.h"

#include <stdbool.h>

// returns whether the fnIndex is a valid index
bool vm_IoFnCall(vm_state_t * state, vm_uint fnIndex, bool peek);

#endif
