//
// Created by David Sullivan on 11/17/22.
//

#ifndef CAVE_ERROR_H
#define CAVE_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CaveError {
    CAVE_NO_ERROR = 0,
    CAVE_FILE_ERROR,
    CAVE_DATA_ERROR,
    CAVE_INDEX_ERROR,
    CAVE_COUNT_ERROR,
    CAVE_MISSING_KEY_ERROR,
//    FORMAT_ERROR,
    CAVE_INSUFFICIENT_MEMORY_ERROR,
    CAVE_COPY_ERROR,
    CAVE_TYPE_ERROR,
    CAVE_INVALID_ARGUMENT_ERROR,

    CAVE_UNKNOWN_ERROR,
} CaveError;

const char* cave_error_string(CaveError e);

#ifdef __cplusplus
}
#endif

#endif //CAVE_ERROR_H
