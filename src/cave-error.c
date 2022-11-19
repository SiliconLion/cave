#include "cave-error.h"

const char* cave_error_string(CAVE_ERROR e) {
    switch (e) {
        case NO_ERROR: {
            return "NO_ERROR";
        }
        case FILE_ERROR: {
            return "FILE_ERROR";
        }
        case DATA_ERROR: {
            return "DATA_ERROR";
        }
        case INSUFFICENT_MEMORY_ERROR: {
            return "INSUFFICENT_MEMORY_ERROR";
        }
        case UNKNOWN_ERROR: {
            return "UNKNOWN_ERROR";
        }
        default: {
            return "value is not a valid error";
        }
    }
}