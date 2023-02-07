#include "basetypes.h"
#include "opcodes.h"
#include "platforminterface.h"
#include "interpreter.h"
#include "state.h"

#include <stdio.h>

VM_DEFINE_IO_INTERFACE(Return1)
{
    UNUSED(state);
    UNUSED(args);
    *outReturn = 100;
}

VM_DEFINE_IO_INTERFACE(Arg1Return1)
{
    UNUSED(state);
    vm_uword_t arg = args[0];
    *outReturn = arg * 3;
}

VM_DEFINE_IO_INTERFACE(Arg3Return1)
{
    UNUSED(state);
    *outReturn = args[0] + args[1] + args[2];
}

VM_DEFINE_IO_INTERFACE(MyPrint)
{
    UNUSED(state);
    UNUSED(outReturn);
    vm_uword_t arg = args[0];
    printf("%d\n", arg);
}

VM_DEFINE_IO_INTERFACE(MyPrint2)
{
    UNUSED(state);
    UNUSED(outReturn);
    vm_uword_t arg0 = args[0];
    vm_uword_t arg1 = args[1];
    printf("%d %d\n", arg0, arg1);
}

// X(name, numArgs, hasReturn)
#define MY_IO_FNS(x) \
    x(Return1, 0, true) x(Arg1Return1, 1, true) x(Arg3Return1, 3, true) \
        x(MyPrint, 1, false) x(MyPrint2, 2, false)

#define GEN_IO_FN_REGISTRY_ITEM(name, args, hasReturn) {&name, args, hasReturn},
#define GEN_IO_FN_ENUM_VALUE(name, args, hasReturn) FN_##name,

vm_ioFunctionRegistryItem_t fnRegistry[] = {MY_IO_FNS(GEN_IO_FN_REGISTRY_ITEM)};
enum
{
    MY_IO_FNS(GEN_IO_FN_ENUM_VALUE)
};

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uword_t * outRegistryListLength)
{
    *outRegistryList = fnRegistry;
    *outRegistryListLength = sizeof(fnRegistry) / sizeof(fnRegistry[0]);
}

#define MEM_SIZE 100
#define STACK_SIZE 100

/*
    X(Return1, 0, true) \
    X(Arg1Return1, 1, true) \
    X(Arg3Return1, 3, true) \
    X(MyPrint, 1, false) \
    X(MyPrint2, 2, false)
*/
vm_uword_t mem[MEM_SIZE] = {
    VM_OP_IO,
    FN_Return1,
    VM_OP_LOADIMM,
    2,
    VM_OP_USUB,
    VM_OP_LOADIMM,
    100,
    VM_OP_LOADIMM,
    1000,
    VM_OP_IO,
    FN_Arg3Return1,
    VM_OP_IO,
    FN_MyPrint,

    VM_OP_LOADIMM,
    10,
    VM_OP_STORE,
    99,

    VM_OP_LOAD, // Addr 17
    99,

    VM_OP_DUP,
    // stack: i i
    VM_OP_LOADIMM,
    10,
    // stack: 10 i i
    VM_OP_SWAP,
    // stack: i 10 i
    VM_OP_USUB,
    // stack: (10-i) i
    VM_PEEK_BITMASK | VM_OP_IO,
    FN_MyPrint2,
    // stack: i
    VM_OP_DROP,

    // Sub 1
    VM_OP_LOADIMM,
    1,
    VM_OP_USUB,

    VM_PEEK_BITMASK | VM_OP_STORE,
    99,

    // Jump if != 0
    VM_OP_LOADIMM,
    0,
    VM_OP_JUMPNEQ,
    17,

    VM_OP_HALT,
};

vm_uword_t stack[STACK_SIZE] = {0};

int main(void)
{
    vm_state_t state;
    vm_InitState(&state, stack, STACK_SIZE, mem, MEM_SIZE);

    vm_programTickResult_t res;
    do
    {
        res = vm_ProcessNextOpcode(&state);
    } while (res == VM_PROCESS_CONTINUE);
}
