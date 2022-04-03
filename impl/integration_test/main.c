#include "vm_BaseTypes.h"
#include "vm_Opcodes.h"
#include "vm_PlatormInterface.h"
#include "vm_ProcessOpcode.h"
#include "vm_State.h"

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
    vm_uint arg = args[0];
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
    vm_uint arg = args[0];
    printf("%d\n", arg);
}

VM_DEFINE_IO_INTERFACE(MyPrint2)
{
    UNUSED(state);
    UNUSED(outReturn);
    vm_uint arg0 = args[0];
    vm_uint arg1 = args[1];
    printf("%d %d\n", arg0, arg1);
}

#define MY_IO_FNS \
    X(Return1, 0, true) \
    X(Arg1Return1, 1, true) \
    X(Arg3Return1, 3, true) \
    X(MyPrint, 1, false) \
    X(MyPrint2, 2, false)

#define X(name, args, hasReturn) {&name, args, hasReturn},
vm_ioFunctionRegistryItem_t fnRegistry[] = {MY_IO_FNS};
#undef X

#define X(name, args, hasReturn) FN_##name,
enum
{
    MY_IO_FNS
};
#undef X

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uint * outRegistryListLength)
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
vm_uint mem[MEM_SIZE] = {
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

vm_uint stack[STACK_SIZE] = {0};

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
