#include "test_Common.h"
#include "vm_BaseTypes.h"
#include "vm_Io.h"
#include "vm_PlatormInterface.h"
#include "vm_State.h"

#include <stdbool.h>
#include <string.h>

TEST_DEFINE_SUITE(my_suite)

#define STACK_SIZE 10
static int stack_idx;
static vm_uint stack[STACK_SIZE];

// MOCKS

void vm_PushStack(vm_state_t * UNUSED_P(state), vm_uint val)
{
    stack_idx--;
    stack[stack_idx] = val;
}

vm_uint vm_PopStack(vm_state_t * UNUSED_P(state))
{
    vm_uint ret = stack[stack_idx];
    stack_idx++;
    ASSERT_MSG(stack_idx <= STACK_SIZE, "Tried to pop off of empty stack");
    return ret;
}

vm_uint vm_PeekStack(vm_state_t * UNUSED_P(state), vm_uint index)
{
    return stack[stack_idx + index];
}

static bool noArgsNoReturnCalled = false;

static VM_DEFINE_IO_INTERFACE(NoArgsNoReturn)
{
    UNUSED(state);
    UNUSED(args);
    UNUSED(outReturn);
    noArgsNoReturnCalled = true;
}

static bool twoArgsOneReturnCalled = false;
static VM_DEFINE_IO_INTERFACE(TwoArgsOneReturn)
{
    UNUSED(state);
    twoArgsOneReturnCalled = true;
    // This must agree with the test cases below
    ASSERT(args[0] == 12);
    ASSERT(args[1] == 34);
    *outReturn = 56;
}

static vm_ioFunctionRegistryItem_t testFnRegistry[2] = {
    {&NoArgsNoReturn, 0, false}, {&TwoArgsOneReturn, 2, true}};

void vmint_GetIoFunctionRegistry(
    vm_ioFunctionRegistryItem_t ** outRegistryList,
    vm_uint * outRegistryListLength)
{
    *outRegistryList = testFnRegistry;
    *outRegistryListLength = sizeof(testFnRegistry) / sizeof(testFnRegistry[0]);
}

static void ResetStack(void)
{
    memset(stack, 0, sizeof(stack));
    stack_idx = STACK_SIZE;
}
static int ItemsOnStack(void) { return STACK_SIZE - stack_idx; }

TEST_DEFINE_CASE(NoArgsNoReturnNoPeek)
{
    ResetStack();
    noArgsNoReturnCalled = false;

    // NOTE this assumes that vm_IoFnCall will never access state directly, but
    // only through mocked functions above
    vm_IoFnCall(NULL, 0, false);

    ASSERT(noArgsNoReturnCalled);
    ASSERT(ItemsOnStack() == 0);
}

TEST_DEFINE_CASE(NoArgsNoReturnPeek)
{
    ResetStack();
    noArgsNoReturnCalled = false;

    // NOTE this assumes that vm_IoFnCall will never access state directly, but
    // only through mocked functions above
    vm_IoFnCall(NULL, 0, true);

    ASSERT(noArgsNoReturnCalled);
    ASSERT(ItemsOnStack() == 0);
}

TEST_DEFINE_CASE(TwoArgsOneReturnNoPeek)
{
    ResetStack();
    twoArgsOneReturnCalled = false;

    // Push args to stack
    stack_idx = STACK_SIZE - 2;
    stack[STACK_SIZE - 1] = 12;
    stack[STACK_SIZE - 2] = 34;

    // NOTE this assumes that vm_IoFnCall will never access state directly, but
    // only through mocked functions above
    vm_IoFnCall(NULL, 1, false);

    ASSERT(twoArgsOneReturnCalled);
    // Return should be on the stack, along with args since they were peeked
    ASSERT(ItemsOnStack() == 1);
    ASSERT(stack[stack_idx] == 56);
}

TEST_DEFINE_CASE(TwoArgsOneReturnPeek)
{
    ResetStack();
    twoArgsOneReturnCalled = false;

    // Push args to stack
    stack_idx = STACK_SIZE - 2;
    stack[STACK_SIZE - 1] = 12;
    stack[STACK_SIZE - 2] = 34;

    // NOTE this assumes that vm_IoFnCall will never access state directly, but
    // only through mocked functions above
    vm_IoFnCall(NULL, 1, true);

    ASSERT(twoArgsOneReturnCalled);
    // Return should be on the stack, along with args since they were peeked
    ASSERT(ItemsOnStack() == 3);
    ASSERT(stack[stack_idx] == 56);
    ASSERT(stack[stack_idx + 1] == 34);
    ASSERT(stack[stack_idx + 2] == 12);
}

int main(void)
{
    test_StartSuite();

    NoArgsNoReturnNoPeek();
    NoArgsNoReturnPeek();
    TwoArgsOneReturnNoPeek();
    TwoArgsOneReturnPeek();

    test_EndSuite();
}
