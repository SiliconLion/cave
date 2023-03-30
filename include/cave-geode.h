//
// Created by David Sullivan on 11/17/22.
//

#ifndef CAVE_GEODE_H
#define CAVE_GEODE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "cave-error.h"
#include "cave-primities.h"
#include "cave-bedrock.h"
#include <stdbool.h>
//#include <stddef.h>


typedef struct CavePolygon {
    CaveVec verts;
} CavePolygon;

CavePolygon* cave_polygon_init(CavePolygon* p, CaveError* err);
void cave_polygon_release(CavePolygon* p);

CavePolygon* cave_polygon_cpy_init(CavePolygon* p, CaveError* err);
cave_2Point* cave_polygon_at(CavePolygon* p, size_t index, CaveError* err);
cave_2Point* cave_polygon_pred(CavePolygon* p, size_t index, CaveError* err);
cave_2Point* cave_polygon_succ(CavePolygon* p, size_t index, CaveError* err);
cave_2Point* cave_polygon_remove_at(CavePolygon* p, size_t index, CaveError* err);

bool cave_is_convex_vert(CavePolygon* p, size_t index, CaveError* err);
bool cave_is_an_ear(CavePolygon* p, size_t index, CaveError* err);
bool cave_is_convex_polygon(CavePolygon* p, CaveError* err);

//straightforward earclipping triangulation. Worst case is O(n^2).
CaveVec cave_kong_triangulate(CavePolygon* p, CaveError* err);

CaveVec cave_toussaint_triangulate(CavePolygon* p, CaveError* err);

bool cave_2d_tri_point_is_inside(cave_2d_Triangle* t, CaveError* err);

/// returns `p1 + p2`
cave_2Point cave_2point_add(cave_2Point p1, cave_2Point p2); 
/// returns `p1 - p2`
cave_2Point cave_2point_sub(cave_2Point p1, cave_2Point p2); 
/// returns length of the vector starting at (0,0), pointing towards `p`
float cave_2d_measure(cave_2Point p);

/// Uses a variation Herron's formula to find the area of a triangle thats *supposed* to be extra stable
float cave_2d_tri_area(cave_2d_Triangle* t, CaveError* err);



#ifdef __cplusplus
}
#endif
#endif //CAVE_GEODE_H