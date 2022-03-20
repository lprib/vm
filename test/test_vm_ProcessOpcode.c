#include "test_Common.h"
#include "vm_BaseTypes.h"
#include "vm_Opcodes.h"
#include "vm_ProcessOpcode.h"

#include <string.h>

// start at 1 since we only mock 2 elements on the stack
static vm_uint stack[100];
static vm_uint mem[100];

static vm_state_t test_state = {
    .pc = mem,
    .sp = stack + 100,
    .stack = stack,
    .stack_size = 100,

    .mem = mem,
    .mem_size = 100};

static void
ResetState(vm_uint mem0, vm_uint mem1, vm_uint stack0, vm_uint stack1)
{
    memset(stack, 0, sizeof(stack));
    memset(mem, 0, sizeof(mem));
    mem[0] = mem0;
    mem[1] = mem1;

    // Items go at the end since stack growns from top down
    stack[98] = stack0;
    stack[99] = stack1;

    test_state.pc = mem;
    test_state.sp = stack + 98;
}

static int ItemsOnStack()
{
    return test_state.stack_size - (test_state.sp - stack);
}
static int MemWordsConsumed() { return test_state.pc - mem; }

static void ProcessNextShouldContinue()
{
    ASSERT(vm_ProcessNextOpcode(&test_state) == VM_PROCESS_CONTINUE);
}

// MOCKS:
// vm_State.h
void vm_PushStack(vm_state_t * state, vm_uint val)
{
    test_state.sp--;
    *test_state.sp = val;
}

vm_uint vm_PopStack(vm_state_t * state)
{
    vm_uint ret = *test_state.sp;
    test_state.sp++;
    return ret;
}

vm_uint vm_PeekStack(vm_state_t * state, vm_uint index)
{
    return test_state.sp[index];
}

vm_uint vm_GetMemAndIncrememt(vm_state_t * state)
{
    vm_uint ret = *test_state.pc;
    test_state.pc++;
    return ret;
}

// vm_Io.h
static int lastIoFnIndex = -1;
static bool functionIsValid;

bool vm_IoFnCall(vm_state_t * state, vm_uint fnIndex)
{
    lastIoFnIndex = fnIndex;
    return functionIsValid;
}

TEST_DEFINE_CASE(Load)
    // Program: Load value at addr 51 to stack
    // Stack: 2 zeros
    ResetState(VM_OPCODE_LOAD, 50, 0, 0);
    mem[50] = 22;
    ProcessNextShouldContinue();

    ASSERT(ItemsOnStack() == 3);
    ASSERT(*test_state.sp == 22);
    ASSERT(MemWordsConsumed() == 2);
}

TEST_DEFINE_CASE(Store)
    ResetState(VM_OPCODE_STORE, 50, 99, 1);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 1);
    // popped 99, now 1 on stack
    ASSERT(*test_state.sp == 1);
    ASSERT(MemWordsConsumed() == 2);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_STORE, 50, 99, 1);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 2);
    // popped 99, now 1 on stack
    ASSERT(*test_state.sp == 99);
    ASSERT(MemWordsConsumed() == 2);
}

TEST_DEFINE_CASE(LoadImm)
    ResetState(VM_OPCODE_LOAD_IMM, 12, 0, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(*test_state.sp == 12);
    ASSERT(MemWordsConsumed() == 2);
}

TEST_DEFINE_CASE(Dup)
    ResetState(VM_OPCODE_DUP, 0, 31, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(*test_state.sp == 31);
    ASSERT(test_state.sp[1] == 31);
    ASSERT(MemWordsConsumed() == 1);
}

TEST_DEFINE_CASE(Swap)
    ResetState(VM_OPCODE_SWAP, 0, 12, 34);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 2);
    ASSERT(test_state.sp[0] == 34);
    ASSERT(test_state.sp[1] == 12);
    ASSERT(MemWordsConsumed() == 1);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_SWAP, 0, 12, 34);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 4);
    ASSERT(test_state.sp[0] == 34);
    ASSERT(test_state.sp[1] == 12);
    ASSERT(test_state.sp[2] == 12);
    ASSERT(test_state.sp[3] == 34);
    ASSERT(MemWordsConsumed() == 1);
}

TEST_DEFINE_CASE(BinaryOps)
    ResetState(VM_OPCODE_ADD, 0, 3, 4);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 1);
    ASSERT(*test_state.sp == 7);
    ASSERT(MemWordsConsumed() == 1);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_ADD, 0, 3, 4);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(test_state.sp[0] == 7);
    ASSERT(test_state.sp[1] == 3);
    ASSERT(test_state.sp[2] == 4);
    ASSERT(MemWordsConsumed() == 1);

    ResetState(VM_OPCODE_USUB, 0, 2, 10);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 1);
    ASSERT(*test_state.sp == 8);
    ASSERT(MemWordsConsumed() == 1);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_USUB, 0, 2, 10);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(test_state.sp[0] == 8);
    ASSERT(test_state.sp[1] == 2);
    ASSERT(test_state.sp[2] == 10);
    ASSERT(MemWordsConsumed() == 1);
}

TEST_DEFINE_CASE(UnaryOps)
    ResetState(VM_OPCODE_INC, 0, 3, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 2);
    ASSERT(*test_state.sp == 4);
    ASSERT(MemWordsConsumed() == 1);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_INC, 0, 3, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(*test_state.sp == 4);
    ASSERT(MemWordsConsumed() == 1);
}

TEST_DEFINE_CASE(JumpsTaken)
    ResetState(VM_OPCODE_JUMP, 55, 0, 0);
    ProcessNextShouldContinue();
    ASSERT(test_state.pc - mem == 55);

    ResetState(VM_OPCODE_JUMPEQ, 55, 5, 5);
    ProcessNextShouldContinue();
    ASSERT(test_state.pc - mem == 55);

    ResetState(VM_OPCODE_JUMPNEQ, 55, 4, 5);
    ProcessNextShouldContinue();
    ASSERT(test_state.pc - mem == 55);

    ResetState(VM_OPCODE_JUMPLT, 55, 3, 2);
    ProcessNextShouldContinue();
    ASSERT(test_state.pc - mem == 55);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_JUMPNEQ, 55, 4, 5);
    ProcessNextShouldContinue();
    ASSERT(test_state.sp[0] == 4);
    ASSERT(test_state.sp[1] == 5);
    ASSERT(test_state.pc - mem == 55);
}

TEST_DEFINE_CASE(JumpsNotTaken)
    ResetState(VM_OPCODE_JUMPEQ, 55, 5, 4);
    ProcessNextShouldContinue();
    ASSERT(MemWordsConsumed() == 2);

    ResetState(VM_OPCODE_JUMPNEQ, 55, 5, 5);
    ProcessNextShouldContinue();
    ASSERT(MemWordsConsumed() == 2);

    ResetState(VM_OPCODE_JUMPLT, 55, 2, 3);
    ProcessNextShouldContinue();
    ASSERT(MemWordsConsumed() == 2);
}

TEST_DEFINE_CASE(Halt)
    ResetState(VM_OPCODE_HALT, 0, 0, 0);
    vm_programTickResult_t res = vm_ProcessNextOpcode(&test_state);
    ASSERT(res == VM_PROCESS_PROGRAM_HALT);
    ASSERT(MemWordsConsumed() == 1);
}

TEST_DEFINE_CASE(IoCallValid)
    ResetState(VM_OPCODE_IO, 10, 0, 0);
    functionIsValid = true;
    ProcessNextShouldContinue();
    ASSERT(lastIoFnIndex == 10);
    ASSERT(MemWordsConsumed() == 2);
}

TEST_DEFINE_CASE(IoCallInvalid)
    ResetState(VM_OPCODE_IO, 5, 0, 0);
    functionIsValid = false;
    vm_programTickResult_t res = vm_ProcessNextOpcode(&test_state);
    ASSERT(lastIoFnIndex == 5);
    ASSERT(res == VM_PROCESS_ERROR_UNDEF_IO_FN);
    ASSERT(MemWordsConsumed() == 2);
}

int main(void)
{
    test_Load();
    test_Store();
    test_LoadImm();
    test_Dup();
    test_Swap();
    test_BinaryOps();
    test_UnaryOps();
    test_JumpsTaken();
    test_JumpsNotTaken();
    test_Halt();
    test_IoCallValid();
    test_IoCallInvalid();
    return 0;
}
