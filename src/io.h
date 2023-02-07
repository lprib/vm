#ifndef VM_IO_H
#define VM_IO_H

#include "basetypes.h"
#include "state.h"

#include <stdbool.h>

// returns whether the fnIndex is a valid index
vm_tick_result_t io_fncall(vm_state_t * state, vm_uword_t fn_index, bool peek);

void io_register(vm_state_t * state, vm_uword_t fn_index, io_fn_spec_t fn_spec);

#endif
