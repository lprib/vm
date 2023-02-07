#include "basetypes.h"
#include "interpreter.h"
#include "opcodes.h"
#include "state.h"
#include "io.h"

#include <stdio.h>

static void
Return1(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    UNUSED(args);
    *outReturn = 100;
}

static void
Arg1Return1(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    vm_uword_t arg = args[0];
    *outReturn = arg * 3;
}

static void
Arg3Return1(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    *outReturn = args[0] + args[1] + args[2];
}

static void
MyPrint(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    UNUSED(outReturn);
    vm_uword_t arg = args[0];
    printf("%d\n", arg);
}

static void
MyPrint2(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    UNUSED(outReturn);
    vm_uword_t arg0 = args[0];
    vm_uword_t arg1 = args[1];
    printf("%d %d\n", arg0, arg1);
}

// X(idx, name, numArgs, hasReturn)
#define MY_IO_FNS(x) \
    x(0, Return1, 0, true) x(1, Arg1Return1, 1, true) \
        x(2, Arg3Return1, 3, true) x(3, MyPrint, 1, false) \
            x(4, MyPrint2, 2, false)

#define GEN_IO_FN_REGISTRY_ITEM(idx, name, args, hasReturn) \
    io_register(&state, idx, (io_fn_spec_t){&name, args, hasReturn});
#define GEN_IO_FN_ENUM_VALUE(idx, name, args, hasReturn) FN_##name = idx,

enum
{
    MY_IO_FNS(GEN_IO_FN_ENUM_VALUE)
};

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
    state_init(&state, stack, STACK_SIZE, mem, MEM_SIZE);

    MY_IO_FNS(GEN_IO_FN_REGISTRY_ITEM)

    vm_tick_result_t res;
    do
    {
        res = interpret_next_op(&state);
    } while (res == VM_PROCESS_CONTINUE);
    printf("%d\n", res);
    printf("%d\n", FN_Return1);
}
