//
// Created by David Sullivan on 11/17/22.
//

#ifndef CAVE_ERROR_H
#define CAVE_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CAVE_ERROR {
    NO_ERROR = 0,
    FILE_ERROR,
    DATA_ERROR,
//    FORMAT_ERROR,
    INSUFFICENT_MEMORY_ERROR,
    UNKNOWN_ERROR,
} CAVE_ERROR;

const char* cave_error_string(CAVE_ERROR e);

#ifdef __cplusplus
}
#endif

#endif //CAVE_ERROR_H
