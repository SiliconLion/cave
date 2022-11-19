//
// Created by David Sullivan on 11/17/22.
//

#include "cave-writer.h"
#include "cave-utilities.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <strings.h>

int read_and_write_STL() {
    printf("testing reading and writing STL files\n");

    FILE* fp = fopen("assets/utah_teapot.stl", "rb");
    if(!fp) {
        printf("invalid file");
        return -1;
    }
    long file_len = cave_file_len(fp);
    uint8_t* file_contents = malloc(file_len);
    if(!file_contents) {
        printf("Not enough memory\n");
        return -1;
    }
    if(file_len != fread(file_contents, 1, file_len, fp) ) {
        printf("Error reading file\n");
        return -1;
    }

    cave_STL_Data teapot_data;
    CAVE_ERROR err;
    err = cave_bytes_to_STL_Data(&teapot_data, file_contents, file_len);
    if(err != NO_ERROR) {
        printf("Error reading stl file: %s\n", cave_error_string(err));
        return -1;
    }

    //the following block creates a null terminated string with the contents of
    //teapot_data.header.
    unsigned char* header_str = calloc(81, 1);
    if(!header_str) {
        printf("Not enough memory\n");
        return -1;
    }
    memcpy(header_str, teapot_data.header, 80);

    printf("Teapot header is the following:\n\"%s\"\n", header_str);
    if(9438 != teapot_data.tri_count) {
        printf("teapot_data has the wrong number of triangles");
        return -1;
    }

    if(cave_Sizeof_STL_Data(&teapot_data) != 84 + (50 * teapot_data.tri_count)) {
        printf("Function `cave_Sizeof_STL_Data(...)` calculated the wrong size.\n");
        return -1;
    }

    if(cave_Sizeof_STL_Data(&teapot_data) != file_len) {
        printf("size discrepancy");
        return -1;
    }

    uint8_t* written_bytes = NULL;
    err = cave_STL_Data_to_Bytes(&written_bytes, &teapot_data);
    if(err != NO_ERROR) {
        printf("function `cave_STL_Data_to_Bytes(...)` returns %s when "
               "`*dest` argument was NULL.\n", cave_error_string(err));
        return -1;
    }


    uint8_t* written_bytes_prealloced = malloc(cave_Sizeof_STL_Data(&teapot_data));
    err = cave_STL_Data_to_Bytes(&written_bytes_prealloced, &teapot_data);
    if(err != NO_ERROR) {
        printf("function `cave_STL_Data_to_Bytes(...)` returns %s when "
               "`*dest` argument was preallocated.\n", cave_error_string(err));
        return -1;
    }

    if(0 != memcmp(file_contents, written_bytes, file_len) ) {
        return -1;
    }
    if(0 != memcmp(file_contents, written_bytes_prealloced, file_len) ) {
        return -1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int test_fails = 0;
    if(0 == read_and_write_STL()) {
        printf("`read_and_write_STL()` passed.\n");
    } else {
        printf("`read_and_write_STL()` failed.\n");
        test_fails += 1;
    }
    return test_fails;
}