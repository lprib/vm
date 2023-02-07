#include "interpreter_driver.h"

#include "basetypes.h"
#include "platforminterface.h"
#include "interpreter.h"
#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VM_DEFINE_IO_INTERFACE(PrintString)
{
    UNUSED(outReturn);
    vm_uword_t addr = args[0];
    vm_uword_t chr = 0;
    while ((chr = state_getmem(state, addr)) != 0)
    {
        printf("%c", chr);
        addr++;
    }
}

VM_DEFINE_IO_INTERFACE(PrintDecimal)
{
    UNUSED(state);
    UNUSED(outReturn);
    printf("%d", args[0]);
}

VM_DEFINE_IO_INTERFACE(PrintChar)
{
    UNUSED(state);
    UNUSED(outReturn);
    printf("%c", args[0]);
}

// X(name, numArgs, hasReturn)
#define IO_FNS(x) \
    x(PrintString, 1, false) x(PrintDecimal, 1, false) x(PrintChar, 1, false)

#define GEN_IO_FN_REGISTRY_ITEM(name, args, hasReturn) {&name, args, hasReturn},

io_fn_spec_t fnRegistry[] = {IO_FNS(GEN_IO_FN_REGISTRY_ITEM)};

void interface_getiofns(
    io_fn_spec_t ** outRegistryList,
    vm_uword_t * outRegistryListLength)
{
    *outRegistryList = fnRegistry;
    *outRegistryListLength = sizeof(fnRegistry) / sizeof(fnRegistry[0]);
}

vm_state_t state;
vm_uword_t * mem;
vm_uword_t * stack;

void li_InitInterpreter(int const mem_size, int const stack_size)
{
    mem = (vm_uword_t *)calloc(mem_size, sizeof(vm_uword_t));
    stack = (vm_uword_t *)calloc(stack_size, sizeof(vm_uword_t));
    state_init(&state, stack, stack_size, mem, mem_size);
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
    if (file_size > (state.mem_size * sizeof(vm_uword_t)))
    {
        fclose(file);
        return LOAD_FAILURE_NOT_ENOUGH_MEM;
    }

    fread(mem, sizeof(vm_uword_t), state.mem_size, file);
    return LOAD_SUCCESS;
}

vm_tick_result_t li_RunProgram(void)
{
    vm_tick_result_t res;
    do
    {
        res = interpret_next_op(&state);
    } while (res == VM_PROCESS_CONTINUE);
    return res;
}

void li_DestroyInterpreter(void)
{
    free(mem);
    free(stack);
}

void li_PrintIoFunctions(void)
{
#define GEN_IO_FN_PRINT_STATEMENT(name, _numArgs, _hasReturn) \
    printf(#name "\n");

    IO_FNS(GEN_IO_FN_PRINT_STATEMENT)
}