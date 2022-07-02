#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "vm_State.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_DEFINE_SUITE(name) \
    static char const _suite_name[] __attribute__((unused)) = #name; \
    static test_assertResultVec_t _assert_results;

#ifdef PRINT_TESTCASE_RUNNING
    #define TEST_DEFINE_CASE(name) TEST_DEFINE_CASE_PRINT_RUNNING(name)
#else
    #define TEST_DEFINE_CASE(name) TEST_DEFINE_CASE_NO_PRINT_RUNNING(name)
#endif

#define test_StartSuite() \
    do \
    { \
        test_initResultVec(&_assert_results); \
    } while (0)

#define test_EndSuite() \
    do \
    { \
        if (_assert_results.size > 0) \
        { \
            printf("Results from %s:\n", _suite_name); \
            test_printResultVec(&_assert_results); \
            test_freeResultVec(&_assert_results); \
        } \
    } while (0)

#define ASSERT_EQ(lhs, rhs) ASSERT_OP((lhs), ==, (rhs))
#define ASSERT_EQ_MSG(lhs, rhs, msg) ASSERT_OP((lhs), ==, (rhs), msg)

#define ASSERT_OP(lhs, op, rhs) ASSERT_OP_MSG((lhs), op, (rhs), DEFAULT_MESSAGE)

#define ASSERT_OP_MSG(lhs, op, rhs, msg) \
    do \
    { \
        typeof(lhs) _lhs = (lhs); \
        typeof(rhs) _rhs = (rhs); \
        if (!((_lhs)op(_rhs))) \
        { \
            char const * _msg = \
                "Expected : '" #lhs " " #op " " #rhs "' (" msg ")"; \
            intptr_t const _lhs_ptr = (intptr_t)_lhs; \
            intptr_t const _rhs_ptr = (intptr_t)_rhs; \
            test_pushResultVec( \
                &_assert_results, \
                (test_assertResult_t){ \
                    .test_fn_name = __func__, \
                    .file_name = __FILE__, \
                    .line_number = __LINE__, \
                    .assert_stringified = _msg, \
                    .type = TEST_ASSERT_OP, \
                    .data1 = _lhs_ptr, \
                    .data2 = _rhs_ptr}); \
        } \
    } while (0)

#define ASSERT(expr) ASSERT_MSG((expr), DEFAULT_MESSAGE)

#define ASSERT_MSG(expr, msg) \
    do \
    { \
        typeof(expr) _expr = (expr); \
        if (!_expr) \
        { \
            char const * _msg = "Expected '" #expr "' to be true (" msg ")"; \
            intptr_t const _expr_ptr = (intptr_t)_expr; \
            test_pushResultVec( \
                &_assert_results, \
                (test_assertResult_t){ \
                    .test_fn_name = __func__, \
                    .file_name = __FILE__, \
                    .line_number = __LINE__, \
                    .assert_stringified = _msg, \
                    .type = TEST_ASSERT_BASIC, \
                    .data1 = _expr_ptr, \
                    .data2 = 0}); \
        } \
    } while (0)

#define TEST_DEFINE_CASE_PRINT_RUNNING(name) \
    static void testcase_##name(void); \
    static void name(void) \
    { \
        printf("Running %s::%s\n", __FILE__, #name); \
        testcase_##name(); \
    } \
    static void testcase_##name(void)

#define TEST_DEFINE_CASE_NO_PRINT_RUNNING(name) \
    static void testcase_##name(void); \
    static void name(void) { testcase_##name(); } \
    static void testcase_##name(void)

#define DEFAULT_MESSAGE "no description"

typedef enum
{
    TEST_ASSERT_OP,
    TEST_ASSERT_BASIC
} test_assertType_t;

typedef struct
{
    char const * test_fn_name;

    char const * file_name;
    int line_number;

    char const * assert_stringified;
    test_assertType_t type;
    intptr_t data1;
    intptr_t data2;
} test_assertResult_t;

typedef struct
{
    test_assertResult_t * buf;
    int size;
    int capacity;
} test_assertResultVec_t;

static inline void test_initResultVec(test_assertResultVec_t * vec)
{
    int const INIT_CAPACITY = 3;
    vec->buf = malloc(sizeof(test_assertResult_t) * INIT_CAPACITY);
    vec->capacity = INIT_CAPACITY;
    vec->size = 0;
}

static inline void
test_pushResultVec(test_assertResultVec_t * vec, test_assertResult_t new_result)
{
    if (vec->size >= vec->capacity)
    {
        int new_capacity = vec->capacity * 2;
        vec->buf =
            realloc(vec->buf, sizeof(test_assertResult_t) * new_capacity);
        vec->capacity = new_capacity;
    }
    vec->buf[vec->size] = new_result;
    vec->size++;
}

static inline void test_printAssert(test_assertResult_t * ass)
{
    printf(
        "%s:%d (in function case %s):\n  %s\n",
        ass->file_name,
        ass->line_number,
        ass->test_fn_name,
        ass->assert_stringified);
    switch (ass->type)
    {
    case TEST_ASSERT_OP:
        printf(
            "  Got lhs=%" PRIiPTR ", rhs=%" PRIiPTR "\n",
            ass->data1,
            ass->data2);
        break;
    case TEST_ASSERT_BASIC:
        printf("  Got val=%" PRIiPTR "\n", ass->data1);
        break;
    }
}

static inline void test_printResultVec(test_assertResultVec_t * vec)
{
    for (int i = 0; i < vec->size; i++)
    {
        test_printAssert(&vec->buf[i]);
    }
}

static inline void test_freeResultVec(test_assertResultVec_t * vec)
{
    // TODO free inner?
    free(vec->buf);
}

#endif
