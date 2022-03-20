#include "vm_BaseTypes.h"
#include "vm_Opcodes.h"
#include "vm_PlatormInterface.h"
#include "vm_ProcessOpcode.h"
#include "vm_State.h"

#include <stdio.h>

VM_DEFINE_IO_INTERFACE(Return1) { *outReturn = 100; }

VM_DEFINE_IO_INTERFACE(Arg1Return1)
{
    vm_uint arg = args[0];
    *outReturn = arg * 3;
}

VM_DEFINE_IO_INTERFACE(Arg3Return1)
{
    *outReturn = args[0] + args[1] + args[2];
}

VM_DEFINE_IO_INTERFACE(MyPrint)
{
    vm_uint arg = args[0];
    printf("%d\n", arg);
}

VM_DEFINE_IO_INTERFACE(MyPrint2)
{
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

vm_uint mem[MEM_SIZE] = {
    VM_OPCODE_IO,
    FN_Return1,
    VM_OPCODE_LOAD_IMM,
    2,
    VM_OPCODE_USUB,
    VM_OPCODE_LOAD_IMM,
    100,
    VM_OPCODE_LOAD_IMM,
    1000,
    VM_OPCODE_IO,
    FN_Arg3Return1,
    VM_OPCODE_IO,
    FN_MyPrint,

    VM_OPCODE_LOAD_IMM,
    10,
    VM_OPCODE_STORE,
    99,

    VM_OPCODE_LOAD, // Addr 17
    99,

    VM_PEEK_BITMASK | VM_OPCODE_IO,
    FN_MyPrint,

    // Sub 1
    VM_OPCODE_LOAD_IMM,
    1,
    VM_OPCODE_USUB,

    VM_PEEK_BITMASK | VM_OPCODE_STORE,
    99,

    // Jump if != 0
    VM_OPCODE_LOAD_IMM,
    0,
    VM_OPCODE_JUMPNEQ,
    17,

    VM_OPCODE_HALT,
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
