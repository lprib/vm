#ifndef VM_INTERPRETER_H
#define VM_INTERPRETER_H

#include "basetypes.h"
#include "state.h"

typedef enum
{
    VM_PROCESS_CONTINUE,
    VM_PROCESS_PROGRAM_HALT,
    VM_PROCESS_ERROR_UNDEF_IO_FN,
    VM_PROCESS_ERROR_INVALID_OPCODE
} vm_tick_result_t;

vm_tick_result_t interpret_next_op(vm_state_t * state);

#endif
