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
        case CAVE_INSUFFICIENT_MEMORY_ERROR: {
            return "CAVE_INSUFFICIENT_MEMORY_ERROR";
        }
        case CAVE_UNKNOWN_ERROR: {
            return "CAVE_UNKNOWN_ERROR";
        }
        default: {
            return "value is not a valid error";
        }
    }
}