//
// Created by David Sullivan on 11/23/22.
//

#include <stdlib.h>
#include <strings.h>
#include "cave-bedrock.h"
#include "cave-utilities.h"


// ><<         ><<><<<<<<<<    ><<
//  ><<       ><< ><<       ><<   ><<
//   ><<     ><<  ><<      ><<
//    ><<   ><<   ><<<<<<  ><<
//     ><< ><<    ><<      ><<
//      ><<<<     ><<       ><<   ><<
//       ><<      ><<<<<<<<   ><<<<


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

CaveVec* cave_vec_append(CaveVec* dest, CaveVec const* src, CaveError* err) {
    if(dest->len + src->len > dest->capacity) {
        void* ret = realloc(dest->data, dest->len + src->len);
        if(!ret) {
            *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
            return NULL;
        }
        dest->data = ret;
    }

    memcpy(
            dest->data + (dest->stride * dest->len), //pointer to the first unused index in dest->data
            src->data,
            src->stride * src->len
            );
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

//   ><<     ><<      ><         ><< <<   ><<     ><<
//   ><<     ><<     >< <<     ><<    ><< ><<     ><<
//   ><<     ><<    ><  ><<     ><<       ><<     ><<
//   ><<<<<< ><<   ><<   ><<      ><<     ><<<<<< ><<
//   ><<     ><<  ><<<<<< ><<        ><<  ><<     ><<
//   ><<     ><< ><<       ><< ><<    ><< ><<     ><<
//   ><<     ><<><<         ><<  ><< <<   ><<     ><<

//uint8_t  cave_hash_uint8(uint8_t x) {
//    uint32_t p = (uint32_t)x;
//    p = (p << 8) + x;
//    p = (p << 8) + x;
//    p = (p << 8) + x;
//
//    uint32_t hash = cave_hash_uint32(x);
//    //safe downcast
//    return (uint8_t) (hash % UINT8_MAX)
//}

//uint16_t cave_hash_uint16(uint16_t x) {
//    uint32_t p = (uint32_t)x;
//    p = (p << 16) + x;
//
//    uint32_t hash = cave_hash_uint32(x);
//    //safe downcast
//    return (uint16_t) (hash % UINT16_MAX)
//}

uint8_t  cave_hash_uint8(uint8_t x) {
    uint32_t hash = cave_hash_uint32(x);
    //safe downcast
    return (uint8_t) (hash % UINT8_MAX);
}

uint16_t cave_hash_uint16(uint16_t x) {
    uint32_t hash = cave_hash_uint32(x);
    //safe downcast
    return (uint16_t) (hash % UINT16_MAX);
}


//implimentation from https://stackoverflow.com/a/12996028/9178974
uint32_t cave_hash_uint32(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

//implimentation from https://stackoverflow.com/a/12996028/9178974
uint64_t cave_hash_uint64(uint64_t x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
}

size_t cave_hash_sizet(size_t x) {
    CAVE_PLATFORM_SWITCH(
            return cave_hash_uint64(x); ,
            return cave_hash_uint32(x);
            )
}


size_t cave_idx_hash_uint8(uint8_t const * x) {
    return cave_hash_sizet( (size_t)*x);
}
size_t cave_idx_hash_uint16(uint16_t const * x) {
    return cave_hash_sizet( (size_t)*x);
}
size_t cave_idx_hash_uint32(uint32_t const * x) {
    return cave_hash_sizet( (size_t)*x);
}
size_t cave_idx_hash_uint64(uint64_t const * x) {
    return cave_hash_sizet( (size_t)*x);
}
size_t cave_idx_hash_sizet(size_t const * x) {
    return cave_hash_sizet( (size_t)*x);
}



const uint8_t ALTERNATING_U8 = 0xAA; // 10101010
const uint32_t ALTERNATING_U16 = 0xAAAA; // 10101010_10101010
const uint32_t ALTERNATING_U32 = 0xAAAAAAAA; // 10101010_10101010_10101010_10101010
const uint64_t ALTERNATING_U64 = 0xAAAAAAAAAAAAAAAA; // 10101010_10101010_10101010_10101010_10101010_10101010_10101010_10101010


size_t cave_hash_bytes(uint8_t const * bytes, size_t byte_count) {
    size_t num_of_full_uint64s = byte_count / sizeof(uint64_t); // truncating division.
    size_t bytes_left = byte_count % sizeof(uint64_t);

    uint8_t const * last_full_uint64 = bytes + (num_of_full_uint64s * sizeof(uint64_t));

    uint64_t accumulator = ALTERNATING_U64;

    uint8_t const * curr_pos = bytes;
    while(curr_pos <= last_full_uint64) {
        uint64_t x = *(uint64_t*)curr_pos;
        uint64_t x_hash = cave_hash_uint64(x);
        accumulator ^= x_hash;

        curr_pos += sizeof(uint64_t);
    }

    uint64_t remaining_bytes = 0;
    curr_pos = last_full_uint64 + sizeof(uint64_t);
    for(size_t i = 0; i < bytes_left; i++) {
        remaining_bytes <<= 8;
        uint8_t byte = *curr_pos;
        remaining_bytes += (uint64_t)byte;
    }


    uint64_t bytes_to_uint64 = sizeof(uint64_t) - bytes_left;
    for(size_t i = 0; i < bytes_to_uint64; i++) {
        remaining_bytes <<= 8;
        remaining_bytes += (uint64_t)ALTERNATING_U8;
    }

    uint64_t remaining_bytes_hash = cave_hash_uint64(remaining_bytes);
    accumulator ^= remaining_bytes;

    //if 32-bit machine, truncating downcast is fine.
    // ToDo: find justification that it definetly truncates or make check for it. Seeing conflicted info.
    return (size_t)cave_hash_uint64(accumulator);
}

size_t cave_hash_str(const char * bytes) {
    return cave_hash_bytes( (uint8_t*)bytes, strlen(bytes));
}



//  ><<     ><<      ><         ><< <<  ><<     ><<    ><<       ><<      ><       ><<<<<<<
//  ><<     ><<     >< <<     ><<    ><<><<     ><<    >< ><<   ><<<     >< <<     ><<    ><<
//  ><<     ><<    ><  ><<     ><<      ><<     ><<    ><< ><< > ><<    ><  ><<    ><<    ><<
//  ><<<<<< ><<   ><<   ><<      ><<    ><<<<<< ><<    ><<  ><<  ><<   ><<   ><<   ><<<<<<<
//  ><<     ><<  ><<<<<< ><<        ><< ><<     ><<    ><<   ><  ><<  ><<<<<< ><<  ><<
//  ><<     ><< ><<       ><< ><<    ><<><<     ><<    ><<       ><< ><<       ><< ><<
//  ><<     ><<><<         ><<  ><< <<  ><<     ><<    ><<       ><<><<         ><<><<



static const size_t CavePrimeList[] = {
        2 , 5 , 11 , 17 , 29 , 47 , 71 , 107 , 163 , 251 ,
        379 , 569 , 857 , 1289 , 1949 , 2927 , 4391 , 6599 ,
        9901 , 14867 , 22303 , 33457 , 50207 , 75323 , 112997 ,
        169501 , 254257 , 381389 , 572087 , 858149 , 1287233 ,
        1930879 , 2896319 , 4344479 , 6516739 , 9775111 , 14662727 ,
        21994111 , 32991187 , 49486793 , 74230231 , 111345347 ,
        167018021 , 250527047 , 375790601 , 563685907 , 845528867 ,
        1268293309 , 1902439967 , 2853659981 , 4280489981 ,

#if UINTPTR_MAX >= UINT64_MAX
        6420734989 ,9631102487 //TODO: Expand this list
#endif
};

//just checking if the system is 64 bit or not. Probably should have a better
//system for that, but this'll do for now.
#if UINTPTR_MAX >= UINT64_MAX
static const size_t CavePrimeListLen = 53;
#else
static const size_t CavePrimeListLen = 51;
#endif



//Copies `key` into `dest->key` using the `h->key_cpy_fn` if there is one, and memcpy if there is not.
//If the `key_cpy_fn` returns an error, `*err` is set, and `NULL` is returned. Otherwise, h is returned.
CaveHashMap* hidden_cave_hashmp_cpy_key_into(CaveHashMap* h, CaveKeyValue* dest, void const * key, CaveError* err) {
    if(!h || !dest || !dest->key || !key) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }
    if (h->key_cpy_fn) {
        CaveError cpy_err = h->value_cpy_fn(dest->key, key);
        if (cpy_err != CAVE_NO_ERROR) {
            *err = cpy_err;
            return NULL;
        } else {
            return h;
        }
    } else {
        memcpy(dest->key, key, h->key_size);
        return h;
    }
}

//Copies `value` into `dest->value` using the `h->value_cpy_fn` if there is one, and memcpy if there is not.
//If the `value_cpy_fn` returns an error, `*err` is set, and `NULL` is returned. Otherwise, h is returned.
CaveHashMap* hidden_cave_hashmp_cpy_value_into(CaveHashMap* h, CaveKeyValue* dest, void const * value, CaveError* err) {
    if(!h || !dest || !value) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }
    if (h->value_cpy_fn) {
        CaveError cpy_err = h->value_cpy_fn(dest->value, value);
        if (cpy_err != CAVE_NO_ERROR) {
            *err = cpy_err;
            return NULL;
        } else {
            return h;
        }
    } else {
        memcpy(dest->value, value, h->value_size);
        return h;
    }
}

bool hidden_cave_hashmp_key_eq(CaveHashMap* h, void const * key_a, void const * key_b) {
    if(h->key_eq_fn) {
        return h->key_eq_fn(key_a, key_b);
    } else {
        int ret = memcmp(key_a, key_b, h->key_size);
        if(ret == 0) {
            return true;
        } else {
            return false;
        }
    }
}

void hidden_cave_destuct_kv(CaveHashMap* h, CaveKeyValue* kv) {
    if(h->kv_destructor) {
        h->kv_destructor(kv);
    } else {
        free(kv->key);
        free(kv->value);
    }
}

/// Finds the index of the first CaveKeyValue that matches `key` within `bucket`. Returns true if found, and
/// false if no CaveKeyValue matches with `key`.
bool hidden_cave_hashmp_kv_index_from_key(size_t* dest, CaveHashMap* h, CaveVec* bucket, void const * key) {
    for(size_t i = 0; i < bucket->len; i++) {
        CaveKeyValue* kv = cave_vec_at_unchecked(bucket, i);
        if(hidden_cave_hashmp_key_eq(h, key, kv->key)) {
            *dest = i;
            return true;
        }
    }

    return false;
}

size_t hidden_cave_get_bucket_index_from_key(CaveHashMap* h, void const * key) {
    size_t hash = h->hash_fn(key);
    return hash % h->capacity;
}

CaveVec* hidden_cave_hashmp_get_bucket(CaveHashMap const * h, void const * key) {
    size_t hash = h->hash_fn(key);
    size_t bucket_index = hash % h->capacity;
    return cave_vec_at_unchecked(&h->buckets, bucket_index);
}

/// \return pointer to CaveKeyValue in the bucket if it is there, and `NULL` if it is not.
CaveKeyValue* hidden_cave_hashmp_key_in_bucket(CaveHashMap* h, CaveVec* bucket, void const * key) {
    for(size_t i = 0; i < bucket->len; i++) {
        CaveKeyValue* kv = cave_vec_at_unchecked(bucket, i);
        if( hidden_cave_hashmp_key_eq(h, kv->key, key) ) {
            return kv;
        }
    }
    return NULL;
}

void cave_simple_free_kv(CaveKeyValue kv) {
    free(kv.key);
    free(kv.value);
}


CaveHashMap* cave_hashmp_init(
        CaveHashMap* h,
        size_t key_size,
        size_t value_size,
        size_t min_capacity,
        CAVE_HASH_FN hash_fn,
        CaveError* err
) {
    if(
            !h || !hash_fn ||
            value_size == 0 || key_size == 0
    ) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    *err = CAVE_NO_ERROR;

    h->value_size = value_size;
    h->key_size = key_size;
    h->hash_fn = hash_fn;
    h->count = 0;

    h->key_eq_fn = NULL;
    h->key_cpy_fn = NULL;
    h->value_cpy_fn = NULL;
    h->kv_destructor = NULL;

    h->capacity = 0;
    for(size_t i = 0; i < CavePrimeListLen; i++) {
        if(CavePrimeList[i] >= min_capacity) {
            h->capacity = CavePrimeList[i];
            break;
        }
    }
    if(h->capacity == 0) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    }

    cave_vec_init(&h->buckets, sizeof(CaveVec), h->capacity, err);
    if(*err != CAVE_NO_ERROR) {
        return NULL;
    }
    for(size_t i = 0; i < h->capacity; i++) {
        CaveVec bucket;
        //we assume the hash function is reasonably good, so initial capacity is minimal per bucket.
        cave_vec_init(&bucket, sizeof(CaveKeyValue), 1, err);
        if(*err != CAVE_NO_ERROR) {
            //it is j < i because this last iteration has not been added to h->buckets.
            for(size_t j = 0; j < i; j++) {
                cave_vec_release(cave_vec_at_unchecked(&h->buckets, i));
            }
            return NULL;
        }
        cave_vec_push(&h->buckets, &bucket, err);
        if(*err != CAVE_NO_ERROR) {
            for(size_t j = 0; j < i; j++) {
                cave_vec_release(cave_vec_at_unchecked(&h->buckets, i));
            }
            //failed to copy the bucket into h->buckets, but bucket was still successfully
            //initialized, so it must be released.
            cave_vec_release(&bucket);
            return NULL;
        }
    }

    //ToDo:
    //arguably there's a more intelligent default capacity to pick, but not obviously to me
    //what that should be, and trying to keep memory overhead low. Perhaps 75% of capacity?
    cave_vec_init(&h->occupied_buckets, sizeof(size_t), 0, err);
    if(*err != CAVE_NO_ERROR) {
        return NULL;
    }
    return h;
}

void cave_hashmp_set_key_eq_fn(CaveHashMap* h, CAVE_KEY_EQ_FN fn) {
    h->key_eq_fn = fn;
}

void cave_hashmp_set_key_cpy_fn(CaveHashMap* h, CAVE_KEY_CPY_FN fn) {
    h->key_cpy_fn = fn;
}

void cave_hashmp_set_value_cpy_fn(CaveHashMap* h, CAVE_VALUE_CPY_FN fn) {
    h->value_cpy_fn = fn;
}

void cave_hashmp_set_kv_destructor_fn(CaveHashMap* h, CAVE_KV_DESTRUCT_FN fn) {
    h->kv_destructor = fn;
}


void cave_hashmp_release(CaveHashMap* h) {
    for(size_t i = 0; i < h->capacity; i++) {
        CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, i);
        cave_vec_release(bucket);
    }
    cave_vec_release(&h->buckets);
    cave_vec_release(&h->occupied_buckets);
}


CaveHashMap* cave_hashmp_insert(CaveHashMap* h, void const * key, void const * value, CaveError* err) {
   if(!h || !key || !value) {
       *err = CAVE_INVALID_ARGUMENT_ERROR;
       return NULL;
   }

    *err = CAVE_NO_ERROR;

    CaveKeyValue new_kv;
    new_kv.key = malloc(h->key_size);
    new_kv.value = malloc(h->value_size);
    if(!new_kv.key || !new_kv.value) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    }

    if(!hidden_cave_hashmp_cpy_key_into(h, &new_kv, key, err)) {
        *err = CAVE_COPY_ERROR;
        return NULL;
    }

    if(!hidden_cave_hashmp_cpy_value_into(h, &new_kv, value, err)) {
        *err = CAVE_COPY_ERROR;
        return NULL;
    }

    size_t bucket_index = hidden_cave_get_bucket_index_from_key(h, key);
    CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
    cave_vec_push(bucket, &new_kv, err);
    if(*err != CAVE_NO_ERROR) { return NULL; }
    cave_vec_push(&h->occupied_buckets, &bucket_index, err);
    if(*err != CAVE_NO_ERROR) { return NULL; }

    h->count += 1;
    return h;
}

CaveHashMap* cave_hashmp_update_or_insert(CaveHashMap* h, void const * key, void const * value, CaveError* err) {
    if(!h || !key || !value) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    *err = CAVE_NO_ERROR;
    CaveVec* bucket = hidden_cave_hashmp_get_bucket(h, key);
    CaveKeyValue* existing_kv = hidden_cave_hashmp_key_in_bucket(h, bucket, key);
    if (existing_kv) {
        return hidden_cave_hashmp_cpy_value_into(h, existing_kv, value, err);
    } else {
        return cave_hashmp_insert(h, key, value, err);
    }
}


void* cave_hashmp_at(CaveHashMap* h, void const * key, CaveError* err) {
    if(!h || !key ) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    *err = CAVE_NO_ERROR;
    CaveVec* bucket = hidden_cave_hashmp_get_bucket(h, key);
    CaveKeyValue* kv = hidden_cave_hashmp_key_in_bucket(h, bucket, key);
    if(kv) {
        return kv->value;
    } else {
        *err = CAVE_MISSING_KEY_ERROR;
        return NULL;
    }
}

CaveHashMap* cave_hashmap_remove(CaveHashMap* h, void const * key, CaveError* err) {
    if(!h || !key ) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    CaveVec* bucket = hidden_cave_hashmp_get_bucket(h, key);
    size_t kv_index;
    if(!hidden_cave_hashmp_kv_index_from_key(&kv_index, h, bucket, key) ) {
        *err = CAVE_MISSING_KEY_ERROR;
        return NULL;
    }
    //TODO: do this in a way that doesnt allocate or copy anything.
    CaveKeyValue* kv = malloc(sizeof(CaveKeyValue));
    cave_vec_remove_at(bucket, kv, kv_index, err); //dont need to check error
    hidden_cave_destuct_kv(h, kv);
    *err = CAVE_NO_ERROR;
    return h;
}

CaveKeyValue* cave_hashmp_cpy_kv_into(CaveKeyValue* dest, CaveHashMap * h, void const * key, CaveError* err) {
    if(!dest || !h || !key) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    *err = CAVE_NO_ERROR;
    CaveVec* bucket = hidden_cave_hashmp_get_bucket(h, key);
    CaveKeyValue* kv = hidden_cave_hashmp_key_in_bucket(h, bucket, key);
    if(!kv) {
        *err = CAVE_MISSING_KEY_ERROR;
        return NULL;
    }
    hidden_cave_hashmp_cpy_key_into(h, dest, kv->key, err);
    hidden_cave_hashmp_cpy_value_into(h, dest, kv->value, err);
    if(*err != CAVE_NO_ERROR) {
        return NULL;
    }
}

CaveKeyValue* cave_hashmp_move_kv_into(CaveKeyValue* dest, CaveHashMap* h, void const * key, CaveError* err) {
    if(!dest || !h || !key) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    CaveVec* bucket = hidden_cave_hashmp_get_bucket(h, key);
    size_t kv_index;
    if(!hidden_cave_hashmp_kv_index_from_key(&kv_index, h, bucket, key)) {
        *err = CAVE_MISSING_KEY_ERROR;
        return NULL;
    }
    memcpy(
            dest,
            cave_vec_at_unchecked(bucket, kv_index),
            sizeof(CaveKeyValue)
            );

    //Todo: unchecked version of this function?
    cave_vec_remove_at(bucket, NULL, kv_index, err);
    h->count -= 1;
    return dest;
}


CaveHashMap* cave_hashmp_clear(CaveHashMap* h) {
    CaveError err = CAVE_NO_ERROR;
    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        size_t bucket_index = *(size_t*)cave_vec_at_unchecked(&h->occupied_buckets, i);
        CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
        for(size_t j = 0; j < bucket->len; j++) {
            CaveKeyValue* kv = (CaveKeyValue*)cave_vec_at_unchecked(bucket, j);
            hidden_cave_destuct_kv(h, kv);
        }


        //bucket is not null, so no need to error check.
        cave_vec_clear(bucket, &err);
    }
    //paramaters are not null, so no need to error check.
    cave_vec_clear(&h->occupied_buckets, &err);
    h->count = 0;
    return h;
}

CaveVec cave_hashmp_cpy_collect(CaveHashMap * h, CaveError* err) {
    if(!h) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        //Todo: add "ZERO_VEC constant/macro?
        return (CaveVec) {.data = NULL, .stride = 0, .capacity = 0, .len = 0};
    }
    CaveVec v;
    cave_vec_init(&v, sizeof(CaveKeyValue), h->count, err);
    if(*err != CAVE_NO_ERROR) {
        return v; //Gotta return something. *shrug*.
    }

    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        size_t bucket_index = *(size_t*)cave_vec_at_unchecked(&h->occupied_buckets, i);
        CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
        for(size_t j = 0; j < bucket->len; j++) {
            CaveKeyValue* kv = cave_vec_at_unchecked(bucket, j);
            CaveKeyValue kv_cpy;
            hidden_cave_hashmp_cpy_key_into(h, &kv_cpy, kv->key, err);
            if(*err != CAVE_NO_ERROR) {
                return v; //Gotta return something. *shrug*.
            }
            hidden_cave_hashmp_cpy_key_into(h, &kv_cpy, kv->value, err);
            if(*err != CAVE_NO_ERROR) {
                return v; //Gotta return something. *shrug*.
            }
            cave_vec_push(&v, &kv_cpy, err);
            if(*err != CAVE_NO_ERROR) {
                return v; //Gotta return something. *shrug*.
            }
        }
    }
    return v;
}


CaveVec cave_hashmp_mv_collect(CaveHashMap* h, CaveError* err) {
    if(!h) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        //Todo: add "ZERO_VEC constant/macro?
        return (CaveVec) {.data = NULL, .stride = 0, .capacity = 0, .len = 0};
    }

    CaveVec v;
    cave_vec_init(&v, sizeof(CaveKeyValue), h->count, err);
    if(*err != CAVE_NO_ERROR) {
        return v; //Gotta return something. *shrug*.
    }

    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        size_t bucket_index = *(size_t*)cave_vec_at_unchecked(&h->occupied_buckets, i);
        CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
        while(bucket->len > 0) {
            CaveVec kv;
            //todo: maybe add an add_uninitalized fn that just incriments the count of v??
            cave_vec_push(&v, &kv, err);
            if(*err != CAVE_NO_ERROR) {
                return v; // gotta return something.
            }
            cave_vec_pop(
                    bucket,
                    cave_vec_end(&v, err), //dont need to check error here cuz v definitly has len > 0.
                    err //dont need to check err here cuz bucket->len must be greater than 0.
                    );
        }
    }
    return v;
}

CaveHashMap* cave_hashmp_foreach(CaveHashMap * h, CAVE_FOREACH_CLOSURE fn, void* closure_data, CaveError* err) {
   if(!h || !fn ) {
       *err = CAVE_INVALID_ARGUMENT_ERROR;
       return NULL;
   }

    *err = CAVE_NO_ERROR;
    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        size_t bucket_index = *(size_t*)cave_vec_at_unchecked(&h->occupied_buckets, i);
        CaveVec* bucket = cave_vec_at_unchecked(&h->occupied_buckets, bucket_index);
        CaveVec* ret = cave_vec_foreach(
                bucket,
                fn,
                closure_data,
                err
                );
        if(!ret) {
            return NULL;
        }
    }
    return h;
}




/// Takes a `CaveKeyValue` and just `memcpy`'s it into `h` (which is to say, neither
/// `h->key_cpy_fn` nor `h->value_cpy_fn` are called.
//void hidden_cave_memcpy_kv_into(CaveHashMap* h, CaveKeyValue kv, CaveError* err) {
//    size_t bucket_index = h->hash_fn(kv.key) % h->capacity;
//    CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
//    cave_vec_push(bucket, &kv, err);
//    // error is reported in err, there is nothing to return so no reason to check.
//}

//CaveHashMap* cave_hashmp_rehash(CaveHashMap* h, size_t new_min_capacity, CaveError* err) {
//    *err = CAVE_NO_ERROR;
//
//    CaveHashMap new_h;
//    cave_hashmp_init(&new_h, h->value_size, h->key_size, new_min_capacity, h->hash_fn, h->key_eq_fn, err);
//    if(*err != CAVE_NO_ERROR) {
//        return NULL;
//    }
//
//    cave_hashmp_foreach(h, (CAVE_FOREACH_CLOSURE)&hidden_cave_cpy_element_into, &new_h, err);
//    if(*err != CAVE_NO_ERROR) {
//        return NULL;
//    }
//    cave_hashmp_cpy_assign(h, &new_h, err);
//    cave_hashmp_release(&new_h);
//    if(*err != CAVE_NO_ERROR) {
//        return NULL;
//    }
//    return h;
//}

CaveHashMap* cave_hashmp_rehash(CaveHashMap* h, size_t new_min_capacity, CaveError* err) {
    if(!h) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    if(new_min_capacity == 0) {
        new_min_capacity = h->capacity + 1;
    }


    size_t new_capacity = 0;
    for(size_t i = 0; i < CavePrimeListLen; i++) {
        if(CavePrimeList[i] >= new_min_capacity) {
            new_capacity = CavePrimeList[i];
            break;
        }
    }
    if(new_capacity == 0) {
        *err = CAVE_INSUFFICIENT_MEMORY_ERROR;
        return NULL;
    }


    //initialize a new buckets vector with vectors of CaveKeyValue
    CaveVec new_buckets;
    cave_vec_init(&new_buckets, sizeof(CaveVec), new_capacity, err);
    if(*err != CAVE_NO_ERROR) {
        return NULL;
    }
    for(size_t i = 0; i < new_capacity; i++) {
        CaveVec new_bucket;
        cave_vec_init(&new_bucket, sizeof(CaveKeyValue), 1, err);
        if(*err != CAVE_NO_ERROR) {
            goto ERROR_CLEANUP_NEWBUCKETS;
        }
        cave_vec_push(&new_buckets, &new_bucket, err);
        if(*err != CAVE_NO_ERROR) {
            cave_vec_release(&new_bucket);
            goto ERROR_CLEANUP_NEWBUCKETS;
        }
    }


    //recalculates the hash of each CaveKeyValue in each bucket, finds what bucket index that becomes
    //in the new_buckets, and memcpy's it into that new bucket
    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        CaveVec* old_bucket = cave_vec_at_unchecked(&h->buckets, i);

        for(size_t j = 0; j < old_bucket->len; j++) {
            CaveKeyValue* kv = cave_vec_at_unchecked(old_bucket, j);

            size_t new_bucket_index = h->hash_fn(kv->key) % new_capacity;
            CaveVec* new_bucket = cave_vec_at_unchecked(&new_buckets, new_bucket_index);

            //relying on the memcpy behavior of cave_vec_push.
            cave_vec_push(new_bucket, kv, err);
            if(*err != CAVE_NO_ERROR) {
                goto ERROR_CLEANUP_NEWBUCKETS;
            }
        }
    }

    //initializes a new version of occupied_buckets based on the new_buckets.
    //kinda annoying to make a new vec when we "could" just write over the old occupied_buckets vec.
    //but then if we run out of memory while doing that, then that would invalidate h, which we don't want to
    //do.
    CaveVec new_occupied_buckets;
    cave_vec_init(&new_occupied_buckets, sizeof(size_t), 0, err);
    if(*err != CAVE_NO_ERROR) {
        goto ERROR_CLEANUP_NEWBUCKETS;
    }
    for(size_t i = 0; i < new_capacity; i++) {
        CaveVec* bucket = cave_vec_at_unchecked(&new_buckets, i);
        if(bucket->len != 0) {
            cave_vec_push(&new_occupied_buckets, &i, err);
            if(*err != CAVE_NO_ERROR) {
                goto ERROR_CLEANUP_NEWOCCUPIEDBUCKETS;
            }
        }
    }

    cave_vec_release(&h->buckets);
    cave_vec_clear(&h->occupied_buckets, err); //occupied buckets is not null so not checking error

    h->buckets = new_buckets;
    h->occupied_buckets = new_occupied_buckets;
    h->capacity = new_capacity;
    return h;

    //Error handling

    ERROR_CLEANUP_NEWOCCUPIEDBUCKETS:
    cave_vec_release(&new_occupied_buckets);
    ERROR_CLEANUP_NEWBUCKETS:
    for(size_t i = 0; i < new_buckets.len; i++) {
        CaveVec* b = cave_vec_at_unchecked(&new_buckets, i);
        cave_vec_release(b);
    }
    cave_vec_release(&new_buckets);
    return NULL;
}

size_t cave_hashmp_total_collisions(CaveHashMap * h) {
    size_t collisions = 0;
    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        size_t bucket_index = *(size_t*)cave_vec_at_unchecked(&h->occupied_buckets, i);
        CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
        collisions += bucket->len;
    }
    return collisions;
}

size_t cave_hashmp_max_collisions(CaveHashMap * h, void* key_dest, CaveError* err) {
    size_t max_collisions = 0;
    CaveVec* bucket_with_max_collisions = NULL;
    for(size_t i = 0; i < h->occupied_buckets.len; i++) {
        size_t bucket_index = *(size_t*)cave_vec_at_unchecked(&h->occupied_buckets, i);
        CaveVec* bucket = cave_vec_at_unchecked(&h->buckets, bucket_index);
        if(bucket->len > max_collisions) {
            max_collisions = bucket->len;
            bucket_with_max_collisions = bucket;
        }
    }
    if(bucket_with_max_collisions && key_dest) {
        hidden_cave_hashmp_cpy_key_into(
                h,
                key_dest,
                cave_vec_at_unchecked(bucket_with_max_collisions, 0),
                err
        );
    }
    return max_collisions;
}


void hidden_cave_hashmp_insert_wrapper(CaveKeyValue* element, CaveHashMap* h, CaveError* err) {
    cave_hashmp_insert(h, element->key, element->value, err);
}

CaveHashMap* cave_hashmp_cpy_assign(CaveHashMap* dest, CaveHashMap * src, CaveError* err) {
    if(!dest || !src) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    *err = CAVE_NO_ERROR;
    bool compatible =
            src->key_size == dest->key_size &&
            src->value_size == dest->value_size;
    if(!compatible) {
        *err = CAVE_TYPE_ERROR;
    }

    CaveHashMap* ret = cave_hashmp_foreach(
            src,
            (CAVE_FOREACH_CLOSURE) hidden_cave_hashmp_insert_wrapper,
            dest,
            err
            );
    if(!ret) {
        return NULL;
    }
    return dest;
}

CaveHashMap* cave_hashmp_cpy_init(CaveHashMap* dest, CaveHashMap * src, CaveError* err) {
    if(!dest || !src) {
        *err = CAVE_INVALID_ARGUMENT_ERROR;
        return NULL;
    }

    cave_hashmp_init(
            dest,
            src->value_size,
            src->key_size,
            src->capacity,
            src->hash_fn,
            err
            );
    if(*err != CAVE_NO_ERROR) {
        return NULL;
    }
    cave_hashmp_set_key_eq_fn(dest, src->key_eq_fn);
    cave_hashmp_set_key_cpy_fn(dest, src->key_cpy_fn);
    cave_hashmp_set_value_cpy_fn(dest, src->value_cpy_fn);
    cave_hashmp_set_kv_destructor_fn(dest, src->kv_destructor);

    cave_hashmp_cpy_assign(dest, src, err);
    if(*err != CAVE_NO_ERROR) {
        return NULL;
    }
    return dest;
}


