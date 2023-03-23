//
// Created by David Sullivan on 11/28/22.
//

#ifndef CAVE_TEST_UTILITIES_H
#define CAVE_TEST_UTILITIES_H

#include "cave-error.h"

#define CAVE_TERMINAL_RED     "\x1B[31m"
#define CAVE_TERMINAL_GREEN   "\x1B[32m"
#define CAVE_TERMINAL_YELLOW  "\x1B[33m"
#define CAVE_TERMINAL_BLUE    "\x1B[34m"
#define CAVE_TERMINAL_MAGENTA "\x1B[35m"
#define CAVE_TERMINAL_CYAN    "\x1B[36m"
#define CAVE_TERMINAL_WHITE   "\x1B[37m"
#define CAVE_TERMINAL_RESET   "\x1B[0m"


#define RUN_TEST(test_fn, test_fail_counter) \
    do {                                     \
        CaveError cv_e = test_fn();          \
        if(cv_e == CAVE_NO_ERROR) { \
            printf(CAVE_TERMINAL_GREEN #test_fn "() passed.\n" CAVE_TERMINAL_RESET); \
        } else { \
            printf( CAVE_TERMINAL_RED #test_fn "() failed with error %s\n" CAVE_TERMINAL_RESET, cave_error_string(cv_e)); \
            test_fail_counter += 1; \
        } \
    } while(0) 

#define RUN_OUTPUT_TEST(test_fn, test_fail_counter, relative_output_dir) \
    /* #if(CAVE_RUN_OUTPUT_TESTS) */ \
    do {                                     \
        CaveError cv_e = test_fn( (relative_output_dir) );          \
        if(cv_e == CAVE_NO_ERROR) { \
            printf(CAVE_TERMINAL_CYAN #test_fn "() ran and returned no errors.\n" CAVE_TERMINAL_RESET); \
        } else { \
            printf( CAVE_TERMINAL_RED #test_fn "() failed with error %s\n" CAVE_TERMINAL_RESET, cave_error_string(cv_e)); \
            test_fail_counter += 1; \
        } \
    } while(0) 
    /* #endif */


#endif //CAVE_TEST_UTILITIES_H