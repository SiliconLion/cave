//
// Created by David Sullivan on 11/17/22.
//

#include "cave-geode.h"



//bool cave_is_convex_vert(CavePolygon* p, size_t index, CaveError* err) {
//
//}

bool cave_is_an_ear(CavePolygon* p, size_t index, CaveError* err) {
    *err = CAVE_NO_ERROR;

    bool index_vert_is_convex = cave_is_convex_vert(p, index, err);
    if (*err != CAVE_NO_ERROR) {
        return false;
    }
    if(!index_vert_is_convex) {
        return false;
    }

    CaveVec R;
    cave_vec_init(&R, sizeof(size_t), p->verts.len, err);
    if (*err != CAVE_NO_ERROR) {
        return false;
    }

    for(size_t i = 0; i < p->verts.len; i++) {
        bool is_convex = cave_is_convex_vert(p, i, err);
        if (*err != CAVE_NO_ERROR) {
            cave_vec_release(&R);
            return false;
        }
        if (!is_convex) {
            cave_vec_push(&R, &i, err);
            if (*err != CAVE_NO_ERROR) {
                cave_vec_release(&R);
                return false;
            }
        }
    }

    if (R.len == 0) {
        //p is a convex polygon, so every vertex is an ear.
        cave_vec_release(&R);
        return true;
    }

    cave_2Point* Xi = cave_polygon_at(p, index, err);
    cave_2Point* pred_Xi = cave_polygon_pred(p, index, err);
    cave_2Point* succ_Xi = cave_polygon_succ(p, index, err);
    if (*err != CAVE_NO_ERROR) {
        cave_vec_release(&R);
        return false;
    }

    cave_2d_Triangle t = {
        .a = *pred_Xi, .b = *Xi, .c = *succ_Xi
    };

    bool contains_concave = false;
    for(size_t i = 0; i < R.len; i++) {
        //don't need to check the error here. R is not null, and the index is valid.
        cave_2Point* r = cave_vec_at(&R, i, err);

        bool ret = cave_2d_tri_point_is_inside(&t, r, err);
        if (*err != CAVE_NO_ERROR) {
            cave_vec_release(&R);
            return false;
        }
        if(ret == true) {
            contains_concave = true;
            break;
        }
    }
    cave_vec_release(&R);
    return !contains_concave;
}

bool cave_is_convex_polygon(CavePolygon* p, CaveError* err) {
    *err = CAVE_NO_ERROR;
    for(size_t i = 0; i < p->verts.len; i++) {
        bool is_convex = cave_is_convex_vert(p, i, err);
        if (*err != CAVE_NO_ERROR) {
            return false;
        }
        if (!is_convex) {
            return false;
        }
    }
    return true;
}


CaveVec cave_kong_triangulate(CavePolygon* p, CaveError* err){
    *err = CAVE_NO_ERROR;

}


CaveVec cave_toussaint_triangulate(CavePolygon* p, CaveError* err);