#include "test_common.h"
#include "basetypes.h"
#include "state.h"

TEST_DEFINE_SUITE(State)

#include <stdlib.h>

#define STACK_SIZE 100
#define MEM_SIZE 100

static vm_state_t NewTestState(void)
{
    vm_state_t state;

    vm_uint * stack = malloc(STACK_SIZE * sizeof(vm_uint));
    vm_uint * mem = malloc(STACK_SIZE * sizeof(vm_uint));

    vm_InitState(&state, stack, STACK_SIZE, mem, MEM_SIZE);

    return state;
}

static int ItemsOnStack(vm_state_t * state)
{
    return state->stack_size - (state->sp - state->stack);
}

TEST_DEFINE_CASE(Init)
{
    vm_state_t test_state;
    vm_uint const stack_size = 50;
    vm_uint const mem_size = 100;

    vm_uint stack[stack_size];
    vm_uint mem[mem_size];

    test_state.pc = stack + 31;
    test_state.sp = mem + 41;

    vm_InitState(&test_state, stack, stack_size, mem, mem_size);

    ASSERT_MSG(test_state.pc == mem, "PC reset");
    ASSERT_MSG(test_state.sp == stack + stack_size, "SP reset to top");
}

TEST_DEFINE_CASE(PushPop)
{
    vm_state_t state = NewTestState();
    vm_PushStack(&state, 1);
    vm_PushStack(&state, 12);

    ASSERT(vm_PopStack(&state) == 12);

    vm_PushStack(&state, 123);
    vm_PushStack(&state, 1234);

    ASSERT(vm_PopStack(&state) == 1234);
    ASSERT(vm_PopStack(&state) == 123);
    ASSERT(vm_PopStack(&state) == 1);
}

TEST_DEFINE_CASE(GetMemAndIncrement)
{
    vm_state_t state = NewTestState();

    state.mem[0] = 55;
    state.mem[1] = 66;
    state.mem[2] = 77;

    ASSERT(vm_GetProgramAndIncrement(&state) == 55);
    ASSERT(vm_GetProgramAndIncrement(&state) == 66);
    ASSERT(vm_GetProgramAndIncrement(&state) == 77);
}

TEST_DEFINE_CASE(Peek)
{
    vm_state_t state = NewTestState();
    vm_PushStack(&state, 77);
    vm_PushStack(&state, 66);
    vm_PushStack(&state, 55);
    ASSERT(vm_PeekStack(&state, 0) == 55);
    ASSERT(vm_PeekStack(&state, 1) == 66);
    ASSERT(vm_PeekStack(&state, 2) == 77);
}

TEST_DEFINE_CASE(Take)
{
    vm_state_t state = NewTestState();
    vm_PushStack(&state, 77);
    vm_PushStack(&state, 66);
    vm_PushStack(&state, 55);
    ASSERT(vm_TakeStack(&state, 0) == 55);
    ASSERT(vm_PopStack(&state) == 66);
    ASSERT(vm_PopStack(&state) == 77);
    ASSERT(ItemsOnStack(&state) == 0);

    state = NewTestState();
    vm_PushStack(&state, 77);
    vm_PushStack(&state, 66);
    vm_PushStack(&state, 55);
    ASSERT(vm_TakeStack(&state, 1) == 66);
    ASSERT(vm_PopStack(&state) == 55);
    ASSERT(vm_PopStack(&state) == 77);
    ASSERT(ItemsOnStack(&state) == 0);

    state = NewTestState();
    vm_PushStack(&state, 77);
    vm_PushStack(&state, 66);
    vm_PushStack(&state, 55);
    vm_PushStack(&state, 44);
    vm_PushStack(&state, 33);
    ASSERT(vm_TakeStack(&state, 3) == 66);
    ASSERT(vm_PopStack(&state) == 33);
    ASSERT(vm_PopStack(&state) == 44);
    ASSERT(vm_PopStack(&state) == 55);
    ASSERT(vm_PopStack(&state) == 77);
    ASSERT(ItemsOnStack(&state) == 0);
}

int main(void)
{
    test_StartSuite();

    Init();
    PushPop();
    GetMemAndIncrement();
    Peek();
    Take();

    test_EndSuite();
    return 0;
}
