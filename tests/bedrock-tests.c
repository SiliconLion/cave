//
// Created by David Sullivan on 11/24/22.
//

#include "cave-error.h"
#include "cave-bedrock.h"
#include "test-utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <string.h>
//#include <inttypes.h>
//#include <stdlib.h>


CaveError cave_vec_init_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    CaveVec* ret = cave_vec_init(&v1, sizeof(int), 300, &err);
    bool correct =
            err == CAVE_NO_ERROR &&
            v1.data != NULL &&
            v1.stride == sizeof(int) &&
            v1.capacity == 300 &&
            v1.len == 0 &&
            ret == &v1;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    ret = cave_vec_init(NULL, sizeof(int), 400, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    err = CAVE_NO_ERROR;
    CaveVec v2;
    ret = cave_vec_init(&v2, 0, 2000, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    err = CAVE_NO_ERROR;
    CaveVec v3;
    ret = cave_vec_init(&v3, 256, 0, &err);
    correct =
            v3.capacity == CAVE_VEC_DEFAULT_CAPACITY &&
            v3.stride == 256 &&
            v3.data != NULL &&
            v3.len == 0 &&
            ret == &v3 &&
            err == CAVE_NO_ERROR;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    cave_vec_release(&v1);
    //v2 ends up not being sucessfully initialized, so we don't free it.
    cave_vec_release(&v3);

    return CAVE_NO_ERROR;
}

CaveError cave_vec_reserve_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;

    cave_vec_init(&v1, sizeof(int), 1, &err);
    CaveVec* ret = cave_vec_reserve(&v1, 22, &err);
    bool correct =
            err == CAVE_NO_ERROR &&
            ret == &v1 &&
            v1.capacity == 22;
    if(!correct) {return CAVE_DATA_ERROR;}

    ret = cave_vec_reserve(&v1, 11, &err);
    correct =
            err == CAVE_NO_ERROR &&
            ret == &v1 &&
            v1.capacity == 11;
    if(!correct) {return CAVE_DATA_ERROR;}


    for(int i = 0; i < 100; i++) {
        cave_vec_push(&v1, &i, &err);
    }
    size_t capacity = v1.capacity;
    ret = cave_vec_reserve(&v1, 50, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            v1.capacity == capacity;
    if(!correct) {return CAVE_DATA_ERROR;}

    ret = cave_vec_reserve(NULL, 20, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {return CAVE_DATA_ERROR;}

    return CAVE_NO_ERROR;
}

CaveError cave_vec_shrink_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(unsigned long), 1024, &err);
    for(unsigned long i = 0; i < 1000; i++) {
        cave_vec_push(&v1, &i, &err);
    }
    CaveVec* ret = cave_vec_shrink(&v1, &err);
    bool correct =
            err == CAVE_NO_ERROR &&
            ret == &v1 &&
            v1.capacity == 1000 &&
            v1.len == 1000;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }


//testing when len == capacity
    CaveVec v2;
    cave_vec_init(&v2, sizeof(unsigned long), 64, &err);
    for(unsigned long i = 0; i < 64; i++) {
        cave_vec_push(&v2, &i, &err);
    }
    ret = cave_vec_shrink(&v2, &err);
    correct =
            err == CAVE_NO_ERROR &&
            ret == &v2 &&
            v2.capacity == 64 &&
            v2.len == 64;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//testing when v is null
    ret = cave_vec_shrink(NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    return CAVE_NO_ERROR;
}

CaveError cave_vec_push_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(int), 256, &err);
    int element = 5;
    CaveVec* ret = cave_vec_push(&v1, &element, &err);
    bool correct =
            err == CAVE_NO_ERROR &&
            v1.len == 1 &&
            v1.capacity == 256 &&
            *(int*)cave_vec_at(&v1, 0, &err) == 5 &&
            ret == &v1;

    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    ret = cave_vec_push(NULL, &element, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    element = 20;
    ret = cave_vec_push(&v1, &element, &err);
    correct =
            err == CAVE_NO_ERROR &&
            v1.len == 2 &&
            *(int*)cave_vec_at(&v1, 0, &err) == 5 &&
            *(int*)cave_vec_at(&v1, 1, &err) == 20 &&
            v1.capacity == 256 &&
            ret == &v1;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    ret = cave_vec_push(&v1, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            v1.len == 2 &&
            *(int*)cave_vec_at(&v1, 0, &err) == 5 &&
            *(int*)cave_vec_at(&v1, 1, &err) == 20 &&
            v1.capacity == 256 &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    ret = cave_vec_push(NULL, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    CaveVec v2;
    ret = cave_vec_init(&v2, sizeof(long), 0, &err);
    correct =
            err == CAVE_NO_ERROR &&
            v2.capacity == CAVE_VEC_DEFAULT_CAPACITY &&
            v2.len == 0 &&
            v2.stride == sizeof(long) &&
            ret == &v2;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    //testing capacity updates correctly
    CaveVec v3;
    cave_vec_init(&v3, sizeof(int), 256, &err);
    int* buffer = malloc(sizeof(int) * ((256 <<2) + 1) );
    for(int i = 0; i < (256 << 2) + 1; i++) {
        buffer[i] = i;
        cave_vec_push(&v3, &i, &err);
        if(err != CAVE_NO_ERROR) {
            return err;
        }
    }

    correct =
            memcmp(v3.data, buffer, (256 << 2) + 1 ) == 0 &&
            v3.len == (256 << 2) + 1 &&
            v3.capacity == 256 << 3;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    cave_vec_release(&v1);
    cave_vec_release(&v2);
    cave_vec_release(&v3);
    return CAVE_NO_ERROR;
}

CaveError cave_vec_at_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(int), 256, &err);
    for(int i = 0; i < 1025; i++) {
        cave_vec_push(&v1, &i, &err);
        if(err != CAVE_NO_ERROR) {
            return err;
        }
    }


    for(int i = 0; i < 1025; i++) {
        int* element = cave_vec_at(&v1, i, &err);
        if(err != CAVE_NO_ERROR) {
            return err;
        }
        if(*element != i) {
            return CAVE_DATA_ERROR;
        }
    }

    int* ret = cave_vec_at(&v1, 1026, &err);
    bool correct =
            err == CAVE_INDEX_ERROR &&
            ret == NULL;

    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    ret = cave_vec_at(&v1, INT32_MAX, &err);
    correct =
            err == CAVE_INDEX_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    CaveVec v2;
    cave_vec_init(&v2, sizeof(long), 0, &err);
    ret = cave_vec_at(&v2, 0, &err);
    correct =
            err == CAVE_INDEX_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    ret = cave_vec_at(NULL, 15, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    cave_vec_release(&v1);
    cave_vec_release(&v2);
    return CAVE_NO_ERROR;
}

CaveError cave_vec_add_at_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(int), 0, &err);
    CaveVec* ret;
    bool correct = true;
//inserting into the start
    size_t prev_len = 0;
    for(int i = 0; i < 2000; i++) {
        prev_len = v1.len;
        ret = cave_vec_add_at(&v1, &i, 0, &err);
        correct =
                ret == &v1 &&
                err == CAVE_NO_ERROR &&
                v1.len == prev_len + 1;
        if(!correct) {
            return CAVE_DATA_ERROR;
        }
    }

//inserting into the middle
    int val = 0;
    ret = cave_vec_add_at(&v1, &val, 1000, &err);
    correct =
            ret == &v1 &&
            err == CAVE_NO_ERROR &&
            v1.len == 2001;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    int predicted_value;
    for(int i = 0; i < 2001; i++) {
        if(i<1000) {
            predicted_value = 2000 - i - 1;
        } else if (i == 1000) {
            predicted_value = 0;
        } else {
            predicted_value = 2000 - i;
        }

        int* element = cave_vec_at(&v1, i, &err);
        if(*element != predicted_value) {
            return CAVE_DATA_ERROR;
        }
    }

//checking invalid conditions
    //invalid index
    prev_len = v1.len;
    ret = cave_vec_add_at(&v1, &val, 5000, &err);
    correct =
            err == CAVE_INDEX_ERROR &&
            ret == NULL &&
            v1.len == prev_len;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    //element is null
    prev_len = v1.len;
    ret = cave_vec_add_at(&v1, NULL, 0, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            v1.len == prev_len;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    //vec is null
    val = 200;
    ret = cave_vec_add_at(NULL, &val, 0, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            val == 200;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    return CAVE_NO_ERROR;
}

CaveError cave_vec_end_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(long), 0,  &err);
    if(err != CAVE_NO_ERROR) {return  err;}
//testing cave_vec_end on a vec with len == 0.
    long* ret = cave_vec_end(&v1, &err);
    bool correct =
            err == CAVE_INDEX_ERROR &&
            ret == NULL;
    if(!correct) {return CAVE_DATA_ERROR;}

    for(long i = 0; i < 10000; i++) {
        cave_vec_push(&v1, &i, &err);
        ret = cave_vec_end(&v1, &err);
        correct =
                err == CAVE_NO_ERROR &&
                *ret == i;
        if(!correct) {return CAVE_DATA_ERROR;}
    }

    ret = cave_vec_end(NULL, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR;
    if(!correct) {return CAVE_DATA_ERROR;}

    cave_vec_release(&v1);
    return CAVE_NO_ERROR;
}

CaveError cave_vec_pop_test() {
    //special cases to test:
    // * null first argument
    // * null second argument
    // * null first and second argument
    // * vec with len 0

    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(long), 0,  &err);
    if(err != CAVE_NO_ERROR) {return  err;}
//testing cave_vec_pop on a vec with len == 0.
    long element = 1234567;
    long* ret = cave_vec_pop(&v1, &element, &err);
    bool correct =
            err == CAVE_INDEX_ERROR &&
            ret == NULL &&
            element == 1234567; // element is not overwritten when cannot pop
    if(!correct) {return CAVE_DATA_ERROR;}

//testing pop works correctly on a filled vector
    for(long i = 0; i < 10000; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    size_t v1_capacity = v1.capacity;

    while(v1.len > 0) {
        ret = cave_vec_pop(&v1, &element, &err);
        correct =
                err == CAVE_NO_ERROR &&
                element == v1.len - 1 &&
                ret == &element &&
                v1.capacity == v1_capacity;
    }

    for(long i = 0; i < 10000; i++) {
        cave_vec_push(&v1, &i, &err);
    }

//testing pop with null second argument
    while(v1.len > 0) {
        ret = cave_vec_pop(&v1, NULL, &err);
        correct =
                err == CAVE_NO_ERROR &&
                element == v1.len - 1 &&
                ret == &element &&
                v1.capacity == v1_capacity;
    }

// testing null first argument
    element = 1234567;
    ret = cave_vec_pop(NULL, &element, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            element == 1234567;
    if(!correct) {return CAVE_DATA_ERROR;}

//testing null first and second argument
    ret = cave_vec_pop(NULL, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {return CAVE_DATA_ERROR;}

    cave_vec_release(&v1);
    return CAVE_NO_ERROR;
}

CaveError cave_vec_remove_at_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(long), 10, &err);

    CaveVec* ret = NULL;
    bool correct = false;

//removing from end
    for(long i = 0; i < 100; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    size_t capacity = v1.capacity;
    long element;

    for(size_t i = 100; i > 0; i--) {
        ret = cave_vec_remove_at(&v1, &element, i - 1, &err);
        correct =
                ret == &v1 &&
                err == CAVE_NO_ERROR &&
                element == i - 1 &&
                v1.len == i - 1 &&
                v1.capacity == capacity &&
                v1.stride == sizeof (long);
        if(!correct) {
            return CAVE_DATA_ERROR;
        }
    }



//remove from start
    for(long i = 0; i < 100; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    for(size_t i = 0; i < 100; i++) {
        ret = cave_vec_remove_at(&v1, &element, 0, &err);
        correct =
                ret == &v1 &&
                err == CAVE_NO_ERROR &&
                element == i &&
                v1.len == 100 - i - 1 &&
                v1.capacity == capacity &&
                v1.stride == sizeof (long);
        if(!correct) {return CAVE_DATA_ERROR;}
    }

//testing incorrect index
    for(long i = 0; i < 100; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    long elem_cpy = element;

    ret = cave_vec_remove_at(&v1, &element, 150, &err);
    correct =
            ret == NULL &&
            err == CAVE_INDEX_ERROR &&
            element == elem_cpy;
    if(!correct) {return CAVE_INDEX_ERROR;}
    ret = cave_vec_remove_at(&v1, &element, 100, &err);
    correct =
            ret == NULL &&
            err == CAVE_INDEX_ERROR &&
            element == elem_cpy;
    if(!correct) {return CAVE_INDEX_ERROR;}


    ret = cave_vec_remove_at(NULL, &element, 15, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR &&
            element == elem_cpy;
    if(!correct) {return CAVE_DATA_ERROR;}

    cave_vec_release(&v1);
    return CAVE_NO_ERROR;
}

CaveError cave_vec_cpy_init_test() {

    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    CaveVec v2;

    cave_vec_init(&v1, sizeof(long), 0, &err);

    for(long i = 0; i < 8000; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    size_t v1_capacity = v1.capacity;
    CaveVec* ret = cave_vec_cpy_init(&v2, &v1, &err);
    bool correct =
            err == CAVE_NO_ERROR &&
            ret == &v2 &&
            v1.len == 8000 &&
            v1.capacity == v1_capacity &&
            v2.stride == v1.stride &&
            v2.len == v1.len &&
            v2.capacity == v1.capacity &&
            memcmp(v1.data, v2.data, sizeof(int) * v1.len) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//dest is null
    cave_vec_release(&v2);
    ret = cave_vec_cpy_init(NULL, &v1, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            v1.len == 8000 &&
            v1.capacity == v1_capacity;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }
//src is null
    ret = cave_vec_cpy_init(&v2, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }
//both are null
    ret = cave_vec_cpy_init(NULL, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    return CAVE_NO_ERROR;
}

CaveError cave_vec_cpy_assign_test() {

    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    CaveVec v2;

    cave_vec_init(&v1, sizeof(long), 200, &err);
    cave_vec_init(&v2, sizeof(long), 400, &err);

    for(long i = 0; i < 200; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    size_t prior_capacity = v1.capacity;
    size_t prior_len = v1.len;

    CaveVec* ret = cave_vec_cpy_assign(&v2, &v1, &err);
    bool correct =
            err == CAVE_NO_ERROR &&
            ret == &v2 &&
            v1.len == prior_len &&
            v1.capacity == prior_capacity &&
            v2.len == v1.len &&
            memcmp(v1.data, v2.data, v1.len * v1.stride) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//testing when dest->capacity < src->len
    CaveVec v3;
    cave_vec_init(&v3, sizeof(long), 1, &err);
    ret = cave_vec_cpy_assign(&v3, &v1, &err);
    correct =
            err == CAVE_NO_ERROR &&
            ret == &v3 &&
            v1.len == prior_len &&
            v1.capacity == prior_capacity &&
            v3.len == v1.len &&
            memcmp(v1.data, v2.data, v1.len * sizeof(long)) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//testing when dest->len != 0
    CaveVec v4;
    CaveVec v5;
    cave_vec_init(&v4, sizeof(long), 400, &err);
    cave_vec_init(&v5, sizeof(long), 0, &err);

    for(long i = 0; i < 10; i++) {
        long element = 99999;
        cave_vec_push(&v4, &element, &err);
    }

    for(long i = 0; i < 400; i++) {
        cave_vec_push(&v5, &i, &err);
    }

    prior_capacity = v4.capacity;
    prior_len = v4.len;

    ret = cave_vec_cpy_assign(&v5, &v4, &err);
    correct =
            err == CAVE_NO_ERROR &&
            ret == &v5 &&
            v4.len == prior_len &&
            v4.capacity == prior_capacity &&
            v5.len == v4.len &&
            memcmp(v4.data, v5.data, v4.len * sizeof(long)) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }


//dest is null
    prior_capacity = v1.capacity;
    prior_len = v1.len;

    ret = cave_vec_cpy_assign(NULL, &v1, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            v1.len == prior_len &&
            v1.capacity == prior_capacity;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//src is null
    ret = cave_vec_cpy_assign(&v1, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL &&
            v1.len == prior_len &&
            v1.capacity == prior_capacity;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//src and dest are NULL
    ret = cave_vec_cpy_assign(NULL, NULL, &err);
    correct =
            err == CAVE_DATA_ERROR &&
            ret == NULL;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }


    cave_vec_release(&v1);
    cave_vec_release(&v2);
    cave_vec_release(&v3);
    return CAVE_NO_ERROR;
}

CaveError cave_vec_clear_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(int), 256, &err);

    for(int i = 0; i < 1025; i++) {
        cave_vec_push(&v1, &i, &err);
        if(err != CAVE_NO_ERROR) {
            return err;
        }
    }

    size_t capacity = v1.capacity;

    CaveVec* ret = cave_vec_clear(&v1, &err);
    bool correct =
            ret == &v1 &&
            err == CAVE_NO_ERROR &&
            v1.len == 0 &&
            v1.capacity == capacity;
    if(!correct) {return CAVE_DATA_ERROR;}

    ret = cave_vec_clear(NULL, &err);
    correct =
            ret == NULL &&
            err == CAVE_DATA_ERROR;

    return CAVE_NO_ERROR;
}

void vec_double_closure(long* element, size_t* count, CaveError* err) {
    *element *= 2;
    *count += 1;
    *err = CAVE_NO_ERROR;
}

void divide_closure(long* element, size_t* count, CaveError* err) {
    if(*element % 2 != 0) {
        //just picking an error that's not in `cave_vec_foreach`
        *err = CAVE_UNKNOWN_ERROR;
        return;
    }
    *element = *element / 2;
    *count += 1;
    return;
}

CaveError cave_vec_foreach_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v;
    cave_vec_init(&v, sizeof(long), 0, &err);

    long buffer[200];
    for(long i = 0; i < 200; i++) {
        cave_vec_push(&v, &i, &err);
        buffer[i] = i*2;
    };

    size_t count = 0;
    CaveVec* ret = cave_vec_foreach(
            &v,
            (CAVE_FOREACH_CLOSURE)vec_double_closure,
            &count,
            &err
            );
    bool correct =
        ret == &v &&
        err == CAVE_NO_ERROR &&
        v.len == 200 &&
        count == 200 &&
        memcmp(buffer, v.data, sizeof(long) * 200) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//closure that will fail;

    //(v is already full of even numbers)
    long* element = cave_vec_at(&v, 100, &err);
    *element = 1;

    for(long i = 0; i < 100; i++) {
        buffer[i] = buffer[i] / 2;
    }
    buffer[100] = 1;

    count = 0;
    ret = cave_vec_foreach(
            &v,
            (CAVE_FOREACH_CLOSURE)divide_closure,
            &count,
            &err
            );
    correct =
            err == CAVE_UNKNOWN_ERROR &&
            ret == NULL &&
            count == 100 &&
            memcmp(v.data, buffer, 200 * sizeof(long)) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    return CAVE_NO_ERROR;
}

bool filter_evens(int const* input_elem, size_t* count, CaveError* err) {
    *err = CAVE_NO_ERROR;
    *count += 1;
    return (*input_elem) % 2 == 0;
}

bool filter_evens_fail_on_neg(int const* input_elem, size_t* count, CaveError* err) {
    if(*input_elem < 0) {
        //just picking an error not found in cave_vec_filter
        *err = CAVE_UNKNOWN_ERROR;
        return false;
    }
    *count += 1;
    return (*input_elem) % 2 == 0;
}

CaveError cave_vec_filter_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v1;
    cave_vec_init(&v1, sizeof(int), 5000, &err);

    for(int i = 0; i < 5000; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    int buffer1[2500];
    for(int i = 0; i < 2500; i++) {
        buffer1[i] = i * 2;
    }

    size_t count = 0;
    CaveVec* ret = cave_vec_filter(
            &v1,
            (CAVE_FILTER_CLOSURE) filter_evens,
            &count,
            &err
            );
    bool correct =
            ret == &v1 &&
            err == CAVE_NO_ERROR &&
            v1.len == 2500 &&
            count == 5000 &&
            memcmp(v1.data, buffer1, sizeof(int) * 2500) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

//filter that fails

    cave_vec_clear(&v1, &err);
    for(int i = 0; i < 5000; i++) {
        cave_vec_push(&v1, &i, &err);
    }

    int* element = cave_vec_at(&v1, 2500, &err);
    *element = -1;

    int buffer2[5000];
    for(int i = 0; i < 5000; i++) {
        buffer2[i] = i;
    }

    count = 0;
    ret = cave_vec_filter(
            &v1,
            (CAVE_FILTER_CLOSURE) filter_evens_fail_on_neg,
            &count,
            &err
    );
    correct =
            err == CAVE_UNKNOWN_ERROR &&
            ret == NULL &&
            count == 2500 &&
            v1.len == 3750 &&
            memcmp(v1.data, buffer1, sizeof(int) * 1250) == 0 &&
            ((int*)v1.data)[1250] == -1 &&
            memcmp(v1.data + (1251 * sizeof(int)), buffer2 + 2501,  2500) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    return CAVE_NO_ERROR;
}

void vec_to_len(CaveVec const * v, size_t* len_output, size_t* total_lens, CaveError* err) {
    *len_output = v->len;
    *total_lens += v->len;
}

void map_even_identity(int const * input_elem, int* output_elm, size_t* count, CaveError*err) {
    if(*input_elem % 2 != 0) {
        //just picking an error that wont be in cave_vec_map
        *err = CAVE_UNKNOWN_ERROR;
        return;
    }
    *count += 1;
    *output_elm = *input_elem;
    *err = CAVE_NO_ERROR;
    return;
}

CaveError cave_vec_map_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec vec_of_vec;
    cave_vec_init(&vec_of_vec, sizeof(CaveVec), 0, &err);

    for(int i = 1; i <= 100; i++) {
        CaveVec elem_vec;
        cave_vec_init(&elem_vec, sizeof(int), i, &err);
        for(int j = 0; j < i; j++) {
            cave_vec_push(&elem_vec, &j, &err);
        }
        cave_vec_push(&vec_of_vec, &elem_vec, &err);
    }

    CaveVec output_vec;
    size_t total_lens = 0;
    CaveVec* ret = cave_vec_map(
            &output_vec,
            &vec_of_vec,
            sizeof(size_t),
            (CAVE_MAP_CLOSURE)vec_to_len,
            &total_lens,
            &err
            );

    if(ret != &output_vec || err != CAVE_NO_ERROR) {
        return CAVE_DATA_ERROR;
    }

    if(output_vec.len != 100) {
        return CAVE_DATA_ERROR;
    }
    for(size_t i = 0; i < output_vec.len; i++) {
        size_t* element = cave_vec_at(&output_vec, i, &err);
        if(*element != i + 1) {
            return CAVE_DATA_ERROR;
        }
    }

    if(total_lens != 5050) {
        return CAVE_DATA_ERROR;
    }

    cave_vec_release(&output_vec);
    for(size_t i = 0; i < vec_of_vec.len; i++) {
        cave_vec_release(cave_vec_at(&vec_of_vec, i, &err));
    }

//map that errors
    CaveVec vec_of_evens;
    cave_vec_init(&vec_of_evens, sizeof(int), 300, &err);
    for(int i = 0; i < 300; i++) {
        int element = i * 2;
        cave_vec_push(&vec_of_evens, &element, &err);
    }

    int* element = cave_vec_at(&vec_of_evens, 100, &err);
    *element = 1;


    size_t count = 0;
    CaveVec dest_vec;
    ret = cave_vec_map(
            &dest_vec,
            &vec_of_evens,
            sizeof(int),
            (CAVE_MAP_CLOSURE) map_even_identity,
            &count,
            &err
            );
    bool correct =
            ret == NULL &&
            err == CAVE_UNKNOWN_ERROR &&
            dest_vec.len == 100 &&
            count == 100 &&
            memcmp(dest_vec.data, vec_of_evens.data, sizeof(int) * 100) == 0;
    if(!correct) {
        return CAVE_DATA_ERROR;
    }

    return CAVE_NO_ERROR;
}

int main(int argc, char* argv[]) {
    int test_fails = 0;

    RUN_TEST(cave_vec_init_test, test_fails);
    RUN_TEST(cave_vec_reserve_test, test_fails);
    RUN_TEST(cave_vec_shrink_test, test_fails);
    RUN_TEST(cave_vec_push_test, test_fails);
    RUN_TEST(cave_vec_at_test, test_fails);
    RUN_TEST(cave_vec_add_at_test, test_fails);
    RUN_TEST(cave_vec_end_test, test_fails);
    RUN_TEST(cave_vec_pop_test, test_fails);
    RUN_TEST(cave_vec_remove_at_test, test_fails);
    RUN_TEST(cave_vec_cpy_init_test, test_fails);
    RUN_TEST(cave_vec_cpy_assign_test, test_fails);
    RUN_TEST(cave_vec_clear_test, test_fails);
    RUN_TEST(cave_vec_foreach_test, test_fails);
    RUN_TEST(cave_vec_filter_test, test_fails);
    RUN_TEST(cave_vec_map_test, test_fails);


    return test_fails;
}