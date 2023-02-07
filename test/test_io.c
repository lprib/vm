#include "test_common.h"
#include "basetypes.h"
#include "io.h"
#include "state.h"
#include "opcodes.h"
#include "interpreter.h"

#include <stdbool.h>
#include <string.h>

TEST_DEFINE_SUITE(my_suite)

static bool noArgsNoReturnCalled = false;

static void NoArgsNoReturn(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    UNUSED(args);
    UNUSED(outReturn);
    noArgsNoReturnCalled = true;
}

static bool twoArgsOneReturnCalled = false;
static void TwoArgsOneReturn(vm_state_t * state, vm_uword_t * args, vm_uword_t * outReturn)
{
    UNUSED(state);
    twoArgsOneReturnCalled = true;
    // This must agree with the test cases below
    ASSERT(args[0] == 12);
    ASSERT(args[1] == 34);
    *outReturn = 56;
}

vm_state_t state;

vm_uword_t stack[100];
vm_uword_t mem[100];

static void ResetState(void)
{
    memset(stack, 0, sizeof(stack));
    memset(mem, 0, sizeof(mem));

    state_init(&state, stack, 100, mem, 100);
    io_register(&state, 0, (io_fn_spec_t){&NoArgsNoReturn, 0, false});
    io_register(&state, 0, (io_fn_spec_t){&TwoArgsOneReturn, 2, true});
}

TEST_DEFINE_CASE(NoArgsNoReturnNoPeek)
{
    ResetState();
    mem[0] = VM_OP_IO;
    mem[1] = 0;
    state_pushstack(&state, 999);

    noArgsNoReturnCalled = false;

    ASSERT_EQ(interpret_next_op(&state), (vm_tick_result_t)VM_PROCESS_CONTINUE);

    ASSERT(noArgsNoReturnCalled);
    ASSERT_EQ(state_getstackusage(&state), 1);
    ASSERT_EQ(state_peekstack(&state, 0), 999);
}

TEST_DEFINE_CASE(NoArgsNoReturnPeek)
{
    ResetState();
    mem[0] = VM_OP_IO | VM_PEEK_BITMASK;
    mem[1] = 0;

    noArgsNoReturnCalled = false;

    ASSERT_EQ(interpret_next_op(&state), (vm_tick_result_t)VM_PROCESS_CONTINUE);

    ASSERT(noArgsNoReturnCalled);
    ASSERT_EQ(state_getstackusage(&state), 0);
}

TEST_DEFINE_CASE(TwoArgsOneReturnNoPeek)
{
    ResetState();
    mem[0] = VM_OP_IO | VM_PEEK_BITMASK;
    mem[1] = 1;

    // Push args to stack
    state_pushstack(&state, 12);
    state_pushstack(&state, 34);
    twoArgsOneReturnCalled = false;

    ASSERT_EQ(interpret_next_op(&state), (vm_tick_result_t)VM_PROCESS_CONTINUE);

    ASSERT(twoArgsOneReturnCalled);
    ASSERT_EQ(state_getstackusage(&state), 1);
    ASSERT_EQ(state_peekstack(&state, 0), 56);
}

TEST_DEFINE_CASE(TwoArgsOneReturnPeek)
{
    ResetState();
    mem[0] = VM_OP_IO | VM_PEEK_BITMASK;
    mem[1] = 1;

    // Push args to stack
    state_pushstack(&state, 12);
    state_pushstack(&state, 34);
    twoArgsOneReturnCalled = false;

    ASSERT_EQ(interpret_next_op(&state), (vm_tick_result_t)VM_PROCESS_CONTINUE);

    ASSERT(twoArgsOneReturnCalled);
    ASSERT_EQ(state_getstackusage(&state), 3);
    ASSERT_EQ(state_popstack(&state), 56);
    ASSERT_EQ(state_popstack(&state), 34);
    ASSERT_EQ(state_popstack(&state), 12);
}

int main(void)
{
    test_startsuite();

    NoArgsNoReturnNoPeek();
    NoArgsNoReturnPeek();
    TwoArgsOneReturnNoPeek();
    TwoArgsOneReturnPeek();

    test_endsuite();
}
