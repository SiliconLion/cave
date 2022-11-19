//
// Created by David Sullivan on 11/17/22.
//

#ifndef CAVE_PRIMITIES_H
#define CAVE_PRIMITIES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct cave_2Point {
    float x;
    float y;
} cave_2Point;

typedef struct cave_3Point {
    float x;
    float y;
    float z;
} cave_3Point;

typedef struct cave_Tex_2Point {
    float x;
    float y;
    float u; // horizontal texture component
    float v; // vertical texture component
} cave_Tex_2Point;

typedef struct cave_Tex_3Point {
    float x;
    float y;
    float z;
    float u; // horizontal texture component
    float v; // vertical texture component
} cave_Tex_3Point;


typedef struct cave_2d_Triangle {
    cave_2Point a;
    cave_2Point b;
    cave_2Point c;
} cave_2d_Triangle;

typedef struct cave_Tex2d_Triangle {
    cave_Tex_2Point a;
    cave_Tex_2Point b;
    cave_Tex_2Point c;
} cave_Tex2d_Triangle;

typedef struct cave_3d_Triangle {
    cave_3Point a;
    cave_3Point b;
    cave_3Point c;
} cave_3d_Triangle;

typedef struct cave_Tex3d_Triangle {
    cave_Tex_3Point a;
    cave_Tex_3Point b;
    cave_Tex_3Point c;
} cave_Tex3d_Triangle;

typedef struct cave_Index_Triangle {
    size_t a;
    size_t b;
    size_t c;
} cave_Index_Triangle ;





#ifdef __cplusplus
}
#endif
#endif //CAVE_CAVE_PRIMITIES_H
