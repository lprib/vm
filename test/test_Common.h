#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "vm_State.h"

#include <assert.h>
#include <stdio.h>

#define ASSERT(test) assert(test)
#define ASSERT_MSG(test, msg) assert(((void)(msg), test))

#define TEST_DEFINE_CASE(name) \
    static void test_##name(void) \
    { \
        printf("%s::%s\n", __FILE__, #name);

#endif
