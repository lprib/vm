#include "test.h"
#include "vm_State.h"

#include <assert.h>
#include <stdint.h>

int main(void)
{
    vm_state_t test_state;
    int const stack_size = 50;
    int const mem_size = 100;

    int16_t stack[stack_size];
    int16_t mem[mem_size];

    test_state.pc = 31;
    test_state.sp = 41;

    vm_InitState(&test_state, stack, stack_size, mem, mem_size);

    ASSERT(test_state.pc == 0, "PC reset");
    ASSERT(test_state.sp == stack_size, "SP reset to top");
}
