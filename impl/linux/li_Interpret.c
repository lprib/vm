#include "li_Interpret.h"

#include "vm_BaseTypes.h"
#include "vm_PlatormInterface.h"
#include "vm_ProcessOpcode.h"
#include "vm_State.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VM_DEFINE_IO_INTERFACE(Print)
{
    UNUSED(state);
    UNUSED(outReturn);
    vm_uint arg = args[0];
    printf("%d\n", arg);
}

// X(name, numArgs, hasReturn)
#define IO_FNS X(Print, 1, false)

#define X(name, args, hasReturn) {&name, args, hasReturn},
vm_ioFunctionRegistryItem_t fnRegistry[] = {IO_FNS};
#undef X

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uint * outRegistryListLength)
{
    *outRegistryList = fnRegistry;
    *outRegistryListLength = sizeof(fnRegistry) / sizeof(fnRegistry[0]);
}

vm_state_t state;
vm_uint * mem;
vm_uint * stack;

void li_InitInterpreter(int const mem_size, int const stack_size)
{
    mem = (vm_uint *)calloc(mem_size, sizeof(vm_uint));
    stack = (vm_uint *)calloc(stack_size, sizeof(vm_uint));
    vm_InitState(&state, stack, stack_size, mem, mem_size);
}

li_loadProgramResult_t li_LoadProgram(char const * filename)
{
    FILE * file = fopen(filename, "rb");
    if (file == NULL)
    {
        return LOAD_FAILURE_COULD_NOT_OPEN_FILE;
    }

    fseek(file, 0, SEEK_SET);
    size_t file_size = ftell(file);
    if (file_size > (state.mem_size * sizeof(vm_uint)))
    {
        fclose(file);
        return LOAD_FAILURE_NOT_ENOUGH_MEM;
    }

    fread(mem, sizeof(vm_uint), state.mem_size, file);
    return LOAD_SUCCESS;
}

void li_RunProgram(void)
{
    while (vm_ProcessNextOpcode(&state) == VM_PROCESS_CONTINUE)
        ;
}

void li_DestroyInterpreter(void)
{
    free(mem);
    free(stack);
}
