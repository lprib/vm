#ifndef VM_INTERPRETER_H
#define VM_INTERPRETER_H

#include "basetypes.h"
#include "state.h"

vm_tick_result_t interpret_next_op(vm_state_t * state);

#endif
