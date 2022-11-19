//
// Created by David Sullivan on 11/17/22.
//

#include "cave-utilities.h"

long cave_file_len(FILE* file){
    fseek(file, 0, SEEK_END);
    long byte_count = ftell(file);
    fseek(file, 0, SEEK_SET);
    return byte_count;
}

