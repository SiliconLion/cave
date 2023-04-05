#include "cave-error.h"

const char* cave_error_string(CaveError e) {
    switch (e) {
        case CAVE_NO_ERROR: {
            return "CAVE_NO_ERROR";
        }
        case CAVE_FILE_ERROR: {
            return "CAVE_FILE_ERROR";
        }
        case CAVE_DATA_ERROR: {
            return "CAVE_DATA_ERROR";
        }
        case CAVE_INDEX_ERROR: {
            return "CAVE_INDEX_ERROR";
        }
        case CAVE_COUNT_ERROR: {
            return "CAVE_COUNT_ERROR";
        }
        case CAVE_ORDER_ERROR: {
            return "CAVE_ORDER_ERROR";
        }
        case CAVE_MISSING_KEY_ERROR: {
            return "CAVE_MISSING_KEY_ERROR";
        }
        case CAVE_INSUFFICIENT_MEMORY_ERROR: {
            return "CAVE_INSUFFICIENT_MEMORY_ERROR";
        }
        case CAVE_COPY_ERROR: {
            return "CAVE_COPY_ERROR";
        }
        case CAVE_TYPE_ERROR: {
            return "CAVE_TYPE_ERROR";
        }
        case CAVE_INVALID_ARGUMENT_ERROR: {
            return "CAVE_INVALID_ARGUMENT_ERROR";
        }
        case CAVE_UNKNOWN_ERROR: {
            return "CAVE_UNKNOWN_ERROR";
        }
        default: {
            return "value is not a valid error";
        }
    }
}