//
// Created by David Sullivan on 11/17/22.
//

#include "cave-utilities.h"

#include <string.h>
#include <stdlib.h>

long cave_file_len(FILE* file){
    fseek(file, 0, SEEK_END);
    long byte_count = ftell(file);
    fseek(file, 0, SEEK_SET);
    return byte_count;
}

char* cave_str_combine_unchecked(const char * first, const char * second) {
    size_t first_len = strlen(first);
    size_t second_len = strlen(second);

    // since calloc zeros out memory, we dont need to explicitly add a null terminator to 
    // properly use strcat on combination
    char* combination = calloc((first_len + second_len + 1), sizeof(char));
    strcat(combination, first);
    strcat(combination + first_len, second);
    return combination;
}