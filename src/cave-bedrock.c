//
// Created by David Sullivan on 11/23/22.
//

#include <stdlib.h>
#include <strings.h>
#include "cave-bedrock.h"

CaveVec* cave_vec_init(CaveVec* v, size_t element_size, size_t initial_capacity, CaveError* err) {
    if(!v || element_size == 0) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    //if initial capacity is 0, then use the default capacity
    size_t capacity = initial_capacity ? initial_capacity : CAVE_VEC_DEFAULT_CAPACITY;
    v->len = 0;
    v->capacity = capacity;
    v->stride = element_size;
    v->data = malloc(element_size * capacity);
    if(!v->data) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    } else {
        *err = CAVE_NO_ERROR;
        return v;
    }
}

CaveVec* cave_vec_reserve(CaveVec* v, size_t capacity, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    if(capacity < v->len) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    void* ret = realloc(v->data, capacity * v->stride);
    if(!ret) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    }
    v->data = ret;
    v->capacity = capacity;
    *err = CAVE_NO_ERROR;
    return v;
}

CaveVec* cave_vec_shrink(CaveVec* v, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    if(v->capacity == v->len) {
        *err = CAVE_NO_ERROR;
        return v;
    }
    void* ret = realloc(v->data, v->len * v->stride);
    if(!ret) {
        *err = CAVE_UNKNOWN_ERROR;
        return NULL;
    }
    v->data = ret;
    v->capacity = v->len;
    *err = CAVE_NO_ERROR;
    return v;
}

void cave_vec_release(CaveVec* v) {
    if(v) {
        free(v->data);
    }
}


CaveVec* cave_vec_push(CaveVec* v, void const* element, CaveError* err) {
    if(!v || !element) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }

    if(v->len == v->capacity) {
        void* ret = realloc(v->data, v->capacity * v->stride * CAVE_VEC_GROW_FACTOR);
        if(!ret) {
            *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
            return NULL;
        }
        v->data = ret;
        v->capacity *= CAVE_VEC_GROW_FACTOR;
    }

    memcpy(v->data + (v->len * v->stride), element, v->stride);
    v->len += 1;
    *err = CAVE_NO_ERROR;
    return v;
}

void* cave_vec_at(CaveVec const* v, size_t index, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    if(index >= v->len) {
        *err = CAVE_INDEX_ERROR;
        return NULL;
    }
    *err = CAVE_NO_ERROR;
    return v->data + (v->stride * index);
}

// void* cave_vec_at_unchecked(CaveVec* v, size_t index) {
//     return v->data + (v->stride * index);
// }

CaveVec* cave_vec_add_at(CaveVec* v, void const* element, size_t index, CaveError* err) {
    if(!v || !element) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    if(index > v->len) {
        *err = CAVE_INDEX_ERROR;
        return NULL;
    }
    if(v->len == v->capacity) {
        void* ret = realloc(v->data, v->stride * v->capacity * CAVE_VEC_GROW_FACTOR);
        if(!ret) {
            *err =  CAVE_INSUFFICIENT_MEMORY_ERROR;
            return NULL;
        }
        v->data = ret;
        v->capacity *= CAVE_VEC_GROW_FACTOR;
    }

    memmove(
            v->data + ((index + 1) * v->stride),
            v->data + index * v->stride,
            v->stride * (v->len - index)
    );
    memcpy(v->data + index * v->stride, element, v->stride);
    v->len += 1;
    *err = CAVE_NO_ERROR;
    return v;
}

void* cave_vec_end(CaveVec const* v, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    return cave_vec_at(v, v->len - 1, err);
}


void* cave_vec_pop(CaveVec* v, void* dest, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    if(v->len == 0) {
        *err = CAVE_INDEX_ERROR;
        return NULL;
    }

    if(dest != NULL) {
        memcpy(dest, v->data + v->len, v->stride);
    }
    v->len -= 1;
    *err = CAVE_NO_ERROR;
    return dest;
}

CaveVec* cave_vec_remove_at(CaveVec* v, void* dest, size_t index, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    if(index >= v->len) {
        *err = CAVE_INDEX_ERROR;
        return NULL;
    }

    if(dest != NULL) {
        memcpy(dest, v->data + (index * v->stride), v->stride);
    }

    if(index == v->len - 1) {
        v->len -= 1;
    } else {
        memmove(
                v->data + (index * v->stride),
                v->data + ((index + 1) * v->stride),
                (v->len - index - 1) * v->stride
                );
        v->len -= 1;
    }

    *err = CAVE_NO_ERROR;
    return v;
}


CaveVec* cave_vec_cpy_init(CaveVec* dest, CaveVec const* src, CaveError* err) {
    if(!dest || !src) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }

    dest->len = src->len;
    dest->stride = src->stride;
    dest->capacity = src->capacity;
    dest->data = malloc(dest->stride * dest->capacity);
    if(!dest->data) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    }
    memcpy(dest->data, src->data, src->len * src->stride);
    *err = CAVE_NO_ERROR;
    return dest;
}

CaveVec* cave_vec_cpy_assign(CaveVec* dest, CaveVec const* src, CaveError* err) {
    if(!dest || !src) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }

    if(dest->capacity < src->len) {
        void* ret = realloc(dest->data, src->len * src->stride);
        if(!ret) {
            *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
            return NULL;
        }
        dest->data = ret;
        dest->capacity = src->len * src->stride;
    }
    memcpy(dest->data, src->data, src->len * src->stride);
    dest->len = src->len;

    *err = CAVE_NO_ERROR;
    return dest;
}

CaveVec* cave_vec_clear(CaveVec* v, CaveError* err) {
    if(!v) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    v->len = 0;
    return v;
}


CaveVec* cave_vec_foreach(CaveVec* v, CAVE_FOREACH_CLOSURE fn, void* closure_data, CaveError* err) {
    if(!v || !fn) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    CaveError serr = CAVE_NO_ERROR;
    for(size_t i = 0; i < v->len; i++) {
        void* element = v->data + (v->stride * i);
        fn(element, closure_data, &serr);
        if(serr != CAVE_NO_ERROR) {
            *err = serr;
            return NULL;
        }
    }
    *err = CAVE_NO_ERROR;
    return v;
}

//this implementation does a lot of copying due to the repeated `cave_vec_remove_at()` calls.
//Could probably find a faster solution, but going with simplicity for now.
CaveVec* cave_vec_filter(CaveVec* v, CAVE_FILTER_CLOSURE fn, void* closure_data, CaveError* err) {
    if(!v || !fn) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }
    CaveError serr = CAVE_NO_ERROR;
    size_t i = 0;
    while(i < v->len) {
        void* element = v->data + (v->stride * i);
        bool keep = fn(element, closure_data, &serr);
        if(serr != CAVE_NO_ERROR) {
            *err = serr;
            return NULL;
        }
        if(!keep) {
            cave_vec_remove_at(v, NULL, i, &serr);
            if(serr != CAVE_NO_ERROR) {
                *err = serr;
                return NULL;
            }
        } else {
            i += 1;
        }
    }
    *err = CAVE_NO_ERROR;
    return v;
}

CaveVec* cave_vec_map(CaveVec* dest, CaveVec const* src, size_t output_stride, CAVE_MAP_CLOSURE fn, void* closure_data, CaveError* err) {
    if(!dest || !src || !fn) {
        *err = CAVE_DATA_ERROR;
        return NULL;
    }

    CaveError serr = CAVE_NO_ERROR;
    cave_vec_init(dest, output_stride, src->capacity, &serr);

    void* out_elem = malloc(output_stride);
    if(!out_elem) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    }
    for(size_t i = 0; i < src->len; i++) {
        void* input_elm = src->data + (src->stride * i);
        fn(input_elm, out_elem, closure_data, &serr);
        if(serr != CAVE_NO_ERROR) {
            *err = serr;
            return NULL;
        }
        cave_vec_push(dest, out_elem, &serr);
        if(serr != CAVE_NO_ERROR) {
            *err = serr;
            return NULL;
        }
    }
    *err = CAVE_NO_ERROR;
    return dest;
}
