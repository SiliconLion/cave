//
// Created by David Sullivan on 11/17/22.
//

#ifndef CAVE_WRITER_H
#define CAVE_WRITER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "cave-primities.h"
#include "cave-error.h"
#include <stdio.h>
#include <stdint.h>

typedef struct cave_STL_Tri {
    cave_3Point normal;
    cave_3Point a;
    cave_3Point b;
    cave_3Point c;
    uint16_t attribute;
} cave_STL_Tri;

typedef struct cave_STL_Data {
    uint8_t header[80];
    uint32_t tri_count;
    cave_STL_Tri* tris;
} cave_STL_Data;

void cave_STL_Data_release(cave_STL_Data* data);

//NOTE: this function doesn't validate the STL data, just reads it in as written. eg, normals may not line up with their triangles.
//`dest` is the location to write the outcome into. `bytes` is the buffer of bytes to parse.
//`bytes_len` is the number of bytes in `bytes`.
//If any error is returned, `*dest` is not valid, but `cave_STL_Data_release(*data)` need not be called.
//Returns `CAVE_DATA_ERROR` if `bytes` describes a malformed STL file,
//or `bytes_len` isn't exactly the number of bytes of the binary STL file.
//If `dest->tri_count == 0`, then `dest->tris` will be `NULL`.
CaveError cave_bytes_to_STL_Data(cave_STL_Data* dest, uint8_t* bytes, size_t bytes_len);

//returns the number of bytes would be needed to write `*data` as an STL binary file.
//returns 0 if `data == NULL`. Otherwise, `*data` is assumed to be well-formed and valid.
size_t cave_Sizeof_STL_Data(cave_STL_Data* data);

//writes `src` to `*dest` following the STL binary file format.
// If `dest == NULL`, then the right number of bytes will be malloc'ed. Otherwise, it is
//assumed `*dest` can properly have the appropriate number of bytes written to it.
CaveError cave_STL_Data_to_Bytes(uint8_t** dest, cave_STL_Data* src);




#ifdef __cplusplus
}
#endif
#endif //CAVE_CAVE_PRIMITIES_H