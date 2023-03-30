//
// Created by David Sullivan on 11/17/22.
//

#include "cave-geode.h"
#include <math.h>


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

cave_2Point cave_2point_add(cave_2Point p1, cave_2Point p2) {
    return (cave_2Point){.x = p1.x + p2.x, .y = p1.y + p2.y};
} 

cave_2Point cave_2point_sub(cave_2Point p1, cave_2Point p2) {
    return (cave_2Point){.x = p1.x - p2.x, .y = p1.y - p2.y};
} 


float cave_2d_measure(cave_2Point p) {
    return sqrtf( (p.x * p.x) + (p.y * p.y) );
}

//if `*v1 > *v2`, then the values in `v1` and `v2` are swapped. Otherwise they are left as is
void hidden_cave_cmp_swap(float* v1, float* v2) {
    if(*v1 < *v2) {
        float temp = *v1;
        *v1 = *v2;
        *v2 = temp;
    }
    return;
}

// algorithm according to https://people.eecs.berkeley.edu/~wkahan/Triangle.pdf 
// via https://scicomp.stackexchange.com/a/27694/45293
float cave_2d_tri_area(cave_2d_Triangle* t, CaveError* err) {
    float side_1 = cave_2d_measure( cave_2point_sub(t->b, t->a) );
    float side_2 = cave_2d_measure( cave_2point_sub(t->b, t->c) );
    float side_3 = cave_2d_measure( cave_2point_sub(t->c, t->a) );

//these are just the side lengths we computed, but ordered s.t. a <= b <= c 
    float a = side_1, b = side_2, c = side_3;
    hidden_cave_cmp_swap(&a, &b); // now a < b
    hidden_cave_cmp_swap(&a, &c); // now a < c
    hidden_cave_cmp_swap(&b, &c); // now b < c
    

    return 0.25 * sqrtf(
        (a + (b + c)) *
        (c - (a - b)) *
        (c + (a - b)) *
        (a + (b - c))
    )
}


