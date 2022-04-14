#ifndef VM_PROCESS_OPCODE_H
#define VM_PROCESS_OPCODE_H

#include "vm_BaseTypes.h"
#include "vm_State.h"

typedef enum
{
    VM_PROCESS_CONTINUE,
    VM_PROCESS_PROGRAM_HALT,
    VM_PROCESS_ERROR_UNDEF_IO_FN,
    VM_PROCESS_ERROR_INVALID_OPCODE
} vm_programTickResult_t;

vm_programTickResult_t vm_ProcessNextOpcode(vm_state_t * state);

#endif
