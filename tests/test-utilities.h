//
// Created by David Sullivan on 11/28/22.
//

#ifndef CAVE_TEST_UTILITIES_H
#define CAVE_TEST_UTILITIES_H

#include "cave-error.h"

#define CAVE_TERMINAL_RED "\x1B[31m"
#define CAVE_TERMINAL_GREEN "\x1B[32m"
#define CAVE_TERMINAL_RESET "\x1B[0m"


#define RUN_TEST(test_fn, test_fail_counter) \
    do {                                     \
        CaveError cv_e = test_fn();          \
        if(cv_e == CAVE_NO_ERROR) { \
            printf(CAVE_TERMINAL_GREEN #test_fn "() passed.\n" CAVE_TERMINAL_RESET); \
        } else { \
            printf( CAVE_TERMINAL_RED #test_fn "() failed with error %s\n" CAVE_TERMINAL_RESET, cave_error_string(cv_e)); \
            test_fail_counter += 1; \
        } \
    } while(0) \


#endif //CAVE_TEST_UTILITIES_H
