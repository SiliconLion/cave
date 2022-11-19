//
// Created by David Sullivan on 11/17/22.
//

#include "cave-writer.h"
#include <stdlib.h>
#include <strings.h>

void hidden_cave_bytes_to_3point(cave_3Point* dest, uint8_t* bytes) {
    memcpy( &(dest->x), bytes, 4);
    memcpy( &(dest->y), bytes + 4, 4);
    memcpy( &(dest->z), bytes + 8, 4);
}

void hidden_cave_3point_to_bytes(uint8_t* dest, cave_3Point src) {
    memcpy(dest, &src.x, 4);
    memcpy(dest + 4, &src.y, 4);
    memcpy(dest + 8, &src.z, 4);
}

void cave_STL_Data_release(cave_STL_Data* data) {
    free(data->tris);
}

CAVE_ERROR cave_bytes_to_STL_Data(cave_STL_Data* dest, uint8_t* bytes, size_t bytes_len) {
    if(bytes_len < 84 | (bytes_len - 84) % 50 != 0) {
        return DATA_ERROR;
    }
    //the first 80 bytes of an STL file is a header filled with whatever the file creator wants
    memcpy( &(dest->header), bytes, 80);
    //the next 4 bytes form an unsigned integer that says how many triangles there are in the file
    memcpy( &(dest->tri_count), bytes + 80, 4);

    //every triangle in the STL format is 50 bytes. (note sizeof(cave_STL_Tri) may not be 50 bytes).
    // integer division ignores remainder, but we already know `(bytes_len - 84) % 50 == 0`, so the
    //following checks are sufficent.
    if(dest->tri_count != (bytes_len - 84) / 50 ) {
        return DATA_ERROR;
    }

    if(dest->tri_count == 0) {
        dest->tris = NULL;
        return NO_ERROR;
    }

    dest->tris = malloc(sizeof(cave_STL_Tri) * dest->tri_count);
    if(!dest->tris) {
        return INSUFFICENT_MEMORY_ERROR;
    }

    uint8_t* triangle_start = bytes + 84;
    for(size_t i = 0; i < dest->tri_count; i++) {
        //again, every triangle in the STL format is 50 bytes.
        uint8_t* curr_pos = triangle_start + (i * 50);
        cave_STL_Tri* curr_tri = dest->tris + i;

        hidden_cave_bytes_to_3point( &(curr_tri->normal), curr_pos);
        hidden_cave_bytes_to_3point( &(curr_tri->a), curr_pos + 12);
        hidden_cave_bytes_to_3point( &(curr_tri->b), curr_pos + 24);
        hidden_cave_bytes_to_3point( &(curr_tri->c), curr_pos + 36);
        memcpy( &curr_tri->attribute, curr_pos + 48, 2);
    }

    return NO_ERROR;
}

size_t cave_Sizeof_STL_Data(cave_STL_Data* data) {
    if(!data) { return 0;}
    return 84 + (data->tri_count * 50);
}

CAVE_ERROR cave_STL_Data_to_Bytes(uint8_t** dest, cave_STL_Data* src) {
    if(!src) {
        return DATA_ERROR;
    }
    if(!dest) {
        return DATA_ERROR;
    } else if(!*dest) {
        *dest = malloc(cave_Sizeof_STL_Data(src));
        if(!*dest) {
            return INSUFFICENT_MEMORY_ERROR;
        }
    }

    uint8_t* bytes = *dest;
    memcpy(bytes, src->header, 80);
    memcpy(bytes+80, &src->tri_count, 4);

    for(size_t i = 0; i < src->tri_count; i++) {
        uint8_t* curr_pos = bytes + 84 + (i * 50);
        cave_STL_Tri* curr_tri = src->tris + i;
        hidden_cave_3point_to_bytes(curr_pos, curr_tri->normal);
        hidden_cave_3point_to_bytes(curr_pos + 12, curr_tri->a);
        hidden_cave_3point_to_bytes(curr_pos + 24, curr_tri->b);
        hidden_cave_3point_to_bytes(curr_pos + 36, curr_tri->c);
        memcpy(curr_pos + 48, &curr_tri->attribute, 2);
    }

    return NO_ERROR;
}