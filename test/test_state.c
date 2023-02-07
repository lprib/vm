#include "basetypes.h"
#include "state.h"
#include "test_common.h"

#include <string.h>

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

#define MY_STACKSIZE 50
#define MY_MEMSIZE 100

TEST_DEFINE_CASE(Init)
{
    vm_state_t test_state;

    vm_uword_t stack[MY_STACKSIZE] = {0};
    vm_uword_t mem[MY_MEMSIZE] = {0};

    test_state.pc = stack + 31;
    test_state.sp = mem + 41;

    state_init(&test_state, stack, MY_STACKSIZE, mem, MY_MEMSIZE);

    ASSERT_EQ(test_state.pc, &mem[0]);
    ASSERT_EQ(test_state.sp, &stack[0] + MY_STACKSIZE);
}

TEST_DEFINE_CASE(PushPop)
{
    vm_state_t state = NewTestState();
    state_pushstack(&state, 1);
    state_pushstack(&state, 12);

    ASSERT_EQ(state_popstack(&state), 12);

    state_pushstack(&state, 123);
    state_pushstack(&state, 1234);

    ASSERT_EQ(state_popstack(&state), 1234);
    ASSERT_EQ(state_popstack(&state), 123);
    ASSERT_EQ(state_popstack(&state), 1);
}

TEST_DEFINE_CASE(GetMemAndIncrement)
{
    vm_state_t state = NewTestState();

    state.mem[0] = 55;
    state.mem[1] = 66;
    state.mem[2] = 77;

    ASSERT_EQ(state_nextinstr(&state), 55);
    ASSERT_EQ(state_nextinstr(&state), 66);
    ASSERT_EQ(state_nextinstr(&state), 77);
}

TEST_DEFINE_CASE(Peek)
{
    vm_state_t state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    ASSERT_EQ(state_peekstack(&state, 0), 55);
    ASSERT_EQ(state_peekstack(&state, 1), 66);
    ASSERT_EQ(state_peekstack(&state, 2), 77);
}

TEST_DEFINE_CASE(Take)
{
    vm_state_t state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    ASSERT_EQ(state_takestack(&state, 0), 55);
    ASSERT_EQ(state_popstack(&state), 66);
    ASSERT_EQ(state_popstack(&state), 77);
    ASSERT_EQ(state_getstackusage(&state), 0);

    state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    ASSERT_EQ(state_takestack(&state, 1), 66);
    ASSERT_EQ(state_popstack(&state), 55);
    ASSERT_EQ(state_popstack(&state), 77);
    ASSERT_EQ(state_getstackusage(&state), 0);

    state = NewTestState();
    state_pushstack(&state, 77);
    state_pushstack(&state, 66);
    state_pushstack(&state, 55);
    state_pushstack(&state, 44);
    state_pushstack(&state, 33);
    ASSERT_EQ(state_takestack(&state, 3), 66);
    ASSERT_EQ(state_popstack(&state), 33);
    ASSERT_EQ(state_popstack(&state), 44);
    ASSERT_EQ(state_popstack(&state), 55);
    ASSERT_EQ(state_popstack(&state), 77);
    ASSERT_EQ(state_getstackusage(&state), 0);
}

TEST_DEFINE_CASE(StackUsage)
{
    vm_state_t state = NewTestState();
    ASSERT_EQ(state_getstackusage(&state), 0);
    state_pushstack(&state, 1);
    ASSERT_EQ(state_getstackusage(&state), 1);
    state_pushstack(&state, 1);
    ASSERT_EQ(state_getstackusage(&state), 2);
    UNUSED(state_popstack(&state));
    ASSERT_EQ(state_getstackusage(&state), 1);
}

int main(void)
{
    test_startsuite();

    Init();
    PushPop();
    GetMemAndIncrement();
    Peek();
    Take();
    StackUsage();

    test_endsuite();
    return 0;
}
