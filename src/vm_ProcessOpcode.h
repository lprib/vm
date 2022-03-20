#ifndef VM_PROCESS_OPCODE_H
#define VM_PROCESS_OPCODE_H

#include "vm_BaseTypes.h"
#include "vm_State.h"

vm_programTickResult_t vm_ProcessNextOpcode(vm_state_t * state);

#endif
