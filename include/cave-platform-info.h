//
// Created by David Sullivan on 2/7/23.
//

#ifndef CAVE_PLATFORM_INFO_H
#define CAVE_PLATFORM_INFO_H

#include <limits.h>

#if CHAR_BIT != 8
    Yikes! system has non 8-bit char
#endif

#if UINTPTR_MAX == UINT64_MAX
    #define CAVE_PLATFORM_64_BIT 1
    #define CAVE_PLATFORM_32_BIT 0
    #define CAVE_PLATFORM_UNSUPPORTED 0
#else
    #if UINTPTR_MAX == UINT32_MAX
        #define CAVE_PLATFORM_64_BIT 0
        #define CAVE_PLATFORM_32_BIT 1
        #define CAVE_PLATFORM_UNSUPPORTED 0
    #else
        #define CAVE_PLATFORM_64_BIT 0
        #define CAVE_PLATFORM_32_BIT 0
        #define CAVE_PLATFORM_UNSUPPORTED 1
    #endif
#endif






#endif //CAVE_PLATFORM_INFO_H
