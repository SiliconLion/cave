//
// Created by David Sullivan on 11/17/22.
//

#ifndef CAVE_UTILITES_H
#define CAVE_UTILITES_H
#ifdef __cplusplus
extern "C" {
#endif

#include "cave-platform-info.h"
#include "cave-error.h"
#include <stdio.h>

long cave_file_len(FILE* file);

///This is lowkey terrible and I'll try to come up with something better at some point.
#define CAVE_PLATFORM_SWITCH(statement_64_bit, statement_32_bit) \
    if(CAVE_PLATFORM_64_BIT) {                                   \
        statement_64_bit                                         \
    } else if(CAVE_PLATFORM_32_BIT) {                            \
        statement_32_bit                                         \
    }


#ifdef CAVE_PLATFORM_64_BIT

#ifdef __cplusplus
}
#endif
#endif //CAVE_UTILITES_H