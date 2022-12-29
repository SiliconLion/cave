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



#ifdef __cplusplus
}
#endif
#endif //CAVE_GEODE_H