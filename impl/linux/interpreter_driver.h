#ifndef LI_INTERPRETER_DRIVER_H
#define LI_INTERPRETER_DRIVER_H

#include "interpreter.h"

typedef enum
{
    LOAD_SUCCESS,
    LOAD_FAILURE_COULD_NOT_OPEN_FILE,
    LOAD_FAILURE_NOT_ENOUGH_MEM
} li_loadProgramResult_t;

void li_InitInterpreter(int const mem_size, int const stack_size);
li_loadProgramResult_t li_LoadProgram(char const * filename);
vm_tick_result_t li_RunProgram(void);
void li_DestroyInterpreter(void);
void li_PrintIoFunctions(void);

#endif
