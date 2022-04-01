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
    test_state.sp = stack + 98;

    test_state.pc = mem;
}

static void ResetStateWithExtendedMemory(
    vm_uint * new_mem, int new_mem_size, vm_uint stack0, vm_uint stack1)
{
    memset(stack, 0, sizeof(stack));
    memset(mem, 0, sizeof(mem));

    // Items go at the end since stack growns from top down
    stack[98] = stack0;
    stack[99] = stack1;
    test_state.sp = stack + 98;

    memcpy(mem, new_mem, new_mem_size);
    test_state.pc = mem;
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
void vm_PushStack(vm_state_t * UNUSED_P(state), vm_uint val)
{
    test_state.sp--;
    *test_state.sp = val;
}

vm_uint vm_PopStack(vm_state_t * UNUSED_P(state))
{
    vm_uint ret = *test_state.sp;
    test_state.sp++;
    return ret;
}

vm_uint vm_PeekStack(vm_state_t * UNUSED_P(state), vm_uint index)
{
    return test_state.sp[index];
}

bool vm_TakeStack_called = false;
vm_uint vm_TakeStack_index = -1;
vm_uint vm_TakeStack(vm_state_t * UNUSED_P(state), vm_uint index)
{
    vm_TakeStack_called = true;
    vm_TakeStack_index = index;
    return 99; // must agree with test case below
}

vm_uint vm_GetProgramAndIncrement(vm_state_t * UNUSED_P(state))
{
    vm_uint ret = *test_state.pc;
    test_state.pc++;
    return ret;
}

vm_uint vm_GetMem(vm_state_t * UNUSED_P(state), vm_uint addr)
{
    return mem[addr];
}

void vm_SetMem(vm_state_t * UNUSED_P(state), vm_uint addr, vm_uint val)
{
    mem[addr] = val;
}

// vm_Io.h
static int lastIoFnIndex = -1;
static bool functionIsValid;

bool vm_IoFnCall(vm_state_t * UNUSED_P(state), vm_uint fnIndex)
{
    lastIoFnIndex = fnIndex;
    return functionIsValid;
}

// TEST CASES:

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

TEST_DEFINE_CASE(Deref)
    ResetState(VM_OPCODE_DEREF, 123, 1, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 2);
    ASSERT(*test_state.sp == 123);
    ASSERT(MemWordsConsumed() == 1);
}

TEST_DEFINE_CASE(ArrayLoad)
    // testing access of array at memory idx 4, struct offset 1, struct size 2,
    // index 2
    vm_uint testmem[] = {
        VM_OPCODE_ARRAY_LOAD,
        4,
        VM_PACK_TO_INT(1, 2),
        0,
        55, // array start, idx 0
        10,
        55, // idx 1
        11,
        55, // idx 2
        12, // idx 2, offset 1
        55,
        13};
    ResetStateWithExtendedMemory(testmem, sizeof(testmem), 2, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 2);
    ASSERT(*test_state.sp == 12);
    ASSERT(MemWordsConsumed() == 3);

    // set peek
    testmem[0] |= VM_PEEK_BITMASK;
    ResetStateWithExtendedMemory(testmem, sizeof(testmem), 2, 0);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(test_state.sp[0] == 12);
    // Should leave index on stack
    ASSERT(test_state.sp[1] == 2);
    ASSERT(MemWordsConsumed() == 3);
}

TEST_DEFINE_CASE(ArrayStore)
    const vm_uint base = 4;
    const vm_uint offset = 2;
    const vm_uint size = 3;
    const vm_uint index = 1;

    vm_uint testmem[] = {
        VM_OPCODE_ARRAY_STORE,
        base,
        VM_PACK_TO_INT(offset, size),
        0,
        1, // array start, idx 0
        1,
        1,
        1, // idx 1,
        1,
        1};
    // value to store = 123, index = 1
    ResetStateWithExtendedMemory(testmem, sizeof(testmem), index, 123);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 0);
    ASSERT(mem[base + index * size + offset] == 123);
    ASSERT(MemWordsConsumed() == 3);

    // set peek
    testmem[0] |= VM_PEEK_BITMASK;
    ResetStateWithExtendedMemory(testmem, sizeof(testmem), index, 123);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 2);
    ASSERT(mem[base + index * size + offset] == 123);
    ASSERT(test_state.sp[0] == index);
    ASSERT(test_state.sp[1] == 123);
    ASSERT(MemWordsConsumed() == 3);
}

TEST_DEFINE_CASE(PickWithoutPeek)
    ResetState(VM_OPCODE_PICK, 56, 12, 34);
    vm_TakeStack_called = false;
    vm_TakeStack_index = -1;
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(vm_TakeStack_called);
    ASSERT(vm_TakeStack_index == 56);
    ASSERT(*test_state.sp == 99);
    ASSERT(MemWordsConsumed() == 2);
}

TEST_DEFINE_CASE(PickWithPeek)
    ResetState(VM_PEEK_BITMASK | VM_OPCODE_PICK, 0, 12, 34);
    vm_TakeStack_called = false;
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(!vm_TakeStack_called);
    ASSERT(test_state.sp[0] == 12);
    ASSERT(test_state.sp[1] == 12);
    ASSERT(test_state.sp[2] == 34);
    ASSERT(MemWordsConsumed() == 2);

    ResetState(VM_PEEK_BITMASK | VM_OPCODE_PICK, 1, 12, 34);
    vm_TakeStack_called = false;
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 3);
    ASSERT(!vm_TakeStack_called);
    ASSERT(test_state.sp[0] == 34);
    ASSERT(test_state.sp[1] == 12);
    ASSERT(test_state.sp[2] == 34);
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

TEST_DEFINE_CASE(Drop)
    ResetState(VM_OPCODE_DROP, 0, 31, 5);
    ProcessNextShouldContinue();
    ASSERT(ItemsOnStack() == 1);
    ASSERT(*test_state.sp == 5);
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

TEST_DEFINE_CASE(Shifts)
    // Note this test is not vm_int bitwidth agnostic (TODO change that)

    ResetState(VM_OPCODE_SHL, 0, 2, 1024);
    ProcessNextShouldContinue();
    ASSERT(*test_state.sp == 4096);

    // sign-extended, stays negative
    ResetState(VM_OPCODE_ASHR, 0, 2, -1024);
    ProcessNextShouldContinue();
    ASSERT((vm_int)(*test_state.sp) == -256);

    // no sign-extend
    ResetState(VM_OPCODE_LSHR, 0, 2, -1024);
    ProcessNextShouldContinue();
    // Does not keep sign bit
    ASSERT((vm_int)(*test_state.sp) == (vm_int)16128);

    // sign-extend
    ResetState(VM_OPCODE_ASHR, 0, 2, 1 << (VM_INT_BITWIDTH - 1));
    ProcessNextShouldContinue();
    // Should arithmetically divide the negative by 4
    ASSERT((vm_int)(*test_state.sp) == (vm_int)(VM_INT_MIN / 4));

    // no sign extend, wraps to INT_MAX
    ResetState(VM_OPCODE_LSHR, 0, 1, 1 << (VM_INT_BITWIDTH - 1));
    ProcessNextShouldContinue();
    ASSERT((vm_int)(*test_state.sp) == (1 << (VM_INT_BITWIDTH - 2)));
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
    test_Deref();
    test_ArrayLoad();
    test_ArrayStore();
    test_PickWithPeek();
    test_PickWithoutPeek();
    test_Dup();
    test_Swap();
    test_Drop();
    test_BinaryOps();
    test_Shifts();
    test_UnaryOps();
    test_JumpsTaken();
    test_JumpsNotTaken();
    test_Halt();
    test_IoCallValid();
    test_IoCallInvalid();
    return 0;
}
