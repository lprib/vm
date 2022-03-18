#include "test.h"
#include "vm_BaseTypes.h"
#include "vm_State.h"

#include <stdlib.h>

TEST_DEFINE_CASE(Init)
    vm_state_t test_state;
    vm_uint const stack_size = 50;
    vm_uint const mem_size = 100;

    vm_uint stack[stack_size];
    vm_uint mem[mem_size];

    test_state.pc = 31;
    test_state.sp = 41;

    vm_InitState(&test_state, stack, stack_size, mem, mem_size);

    ASSERT_MSG(test_state.pc == 0, "PC reset");
    ASSERT_MSG(test_state.sp == stack_size, "SP reset to top");
}

static vm_state_t NewTestState(void)
{
    vm_state_t state;
    vm_uint const stack_size = 100;
    vm_uint const mem_size = 100;

    vm_uint * stack = malloc(stack_size * sizeof(vm_uint));
    vm_uint * mem = malloc(stack_size * sizeof(vm_uint));

    vm_InitState(&state, stack, stack_size, mem, mem_size);

    return state;
}

TEST_DEFINE_CASE(PushPop)
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

int main(void)
{
    test_Init();
    test_PushPop();
}
