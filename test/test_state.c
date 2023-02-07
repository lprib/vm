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

    vm_uword_t * stack = malloc(STACK_SIZE * sizeof(vm_uword_t));
    vm_uword_t * mem = malloc(STACK_SIZE * sizeof(vm_uword_t));

    state_init(&state, stack, STACK_SIZE, mem, MEM_SIZE);

    return state;
}

static int ItemsOnStack(vm_state_t * state)
{
    return state->stack_size - (state->sp - state->stack);
}

TEST_DEFINE_CASE(Init)
{
    vm_state_t test_state;
    vm_uword_t const stack_size = 50;
    vm_uword_t const mem_size = 100;

    vm_uword_t stack[stack_size];
    vm_uword_t mem[mem_size];

    test_state.pc = stack + 31;
    test_state.sp = mem + 41;

    state_init(&test_state, stack, stack_size, mem, mem_size);

    ASSERT_MSG(test_state.pc == mem, "PC reset");
    ASSERT_MSG(test_state.sp == stack + stack_size, "SP reset to top");
}

TEST_DEFINE_CASE(PushPop)
{
    vm_state_t state = NewTestState();
    state_pushstack(&state, 1);
    state_pushstack(&state, 12);

    ASSERT(state_popstack(&state) == 12);

    state_pushstack(&state, 123);
    state_pushstack(&state, 1234);

    ASSERT(state_popstack(&state) == 1234);
    ASSERT(state_popstack(&state) == 123);
    ASSERT(state_popstack(&state) == 1);
}

TEST_DEFINE_CASE(GetMemAndIncrement)
{
    vm_state_t state = NewTestState();

    state.mem[0] = 55;
    state.mem[1] = 66;
    state.mem[2] = 77;

    ASSERT(state_nextinstr(&state) == 55);
    ASSERT(state_nextinstr(&state) == 66);
    ASSERT(state_nextinstr(&state) == 77);
}

TEST_DEFINE_CASE(Peek)
{
    vm_state_t state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    ASSERT(state_peekstack(&state, 0) == 55);
    ASSERT(state_peekstack(&state, 1) == 66);
    ASSERT(state_peekstack(&state, 2) == 77);
}

TEST_DEFINE_CASE(Take)
{
    vm_state_t state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    ASSERT(state_takestack(&state, 0) == 55);
    ASSERT(state_popstack(&state) == 66);
    ASSERT(state_popstack(&state) == 77);
    ASSERT(ItemsOnStack(&state) == 0);

    state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    ASSERT(state_takestack(&state, 1) == 66);
    ASSERT(state_popstack(&state) == 55);
    ASSERT(state_popstack(&state) == 77);
    ASSERT(ItemsOnStack(&state) == 0);

    state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    state_pushstack(&state, 44);
    state_pushstack(&state, 33);
    ASSERT(state_takestack(&state, 3) == 66);
    ASSERT(state_popstack(&state) == 33);
    ASSERT(state_popstack(&state) == 44);
    ASSERT(state_popstack(&state) == 55);
    ASSERT(state_popstack(&state) == 77);
    ASSERT(ItemsOnStack(&state) == 0);
}

int main(void)
{
    test_startsuite();

    Init();
    PushPop();
    GetMemAndIncrement();
    Peek();
    Take();

    test_endsuite();
    return 0;
}
