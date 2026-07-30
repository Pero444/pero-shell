/*
 * mini_test.h — tiny assertion/test-runner framework, no external
 * dependencies. Good enough for a class project: shows you understand what
 * a test framework actually does (count assertions, report failures with
 * file/line, summarize) without pulling in something like Unity/CUnit that
 * would need to be justified/explained/installed separately.
 */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_failed = 0;



#define ASSERT_TRUE(cond)                                                   \
    do {                                                                    \
        tests_run++;                                                        \
        if (!(cond)) {                                                      \
            tests_failed++;                                                 \
            printf("  FAIL (%s:%d): %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                    \
    } while (0)

#define ASSERT_EQ_INT(expected, actual)                                     \
    do {                                                                    \
        tests_run++;                                                        \
        long _e = (long)(expected), _a = (long)(actual);                    \
        if (_e != _a) {                                                     \
            tests_failed++;                                                 \
            printf("  FAIL (%s:%d): expected %ld, got %ld\n", __FILE__,     \
                   __LINE__, _e, _a);                                       \
        }                                                                    \
    } while (0)

#define ASSERT_STR_EQ(expected, actual)                                     \
    do {                                                                    \
        tests_run++;                                                        \
        if (strcmp((expected), (actual)) != 0) {                            \
            tests_failed++;                                                 \
            printf("  FAIL (%s:%d): expected \"%s\", got \"%s\"\n",         \
                   __FILE__, __LINE__, expected, actual);                   \
        }                                                                    \
    } while (0)

#define RUN_TEST(fn)                                                        \
    do {                                                                    \
        printf("RUN  %s\n", #fn);                                          \
        fn();                                                               \
    } while (0)

/* Call once at the end of main(). Returns nonzero (shell-style failure)
 * if any assertion failed, so this doubles as a CI/Makefile exit code. */
static inline int test_summary(void) {
    printf("\n%d/%d assertions passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}

#endif