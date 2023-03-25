//
// Created by David Sullivan on 11/24/22.
//

#include "cave-error.h"
#include "cave-bedrock.h"
#include "cave-utilities.h"
#include "test-utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <inttypes.h>


// ><<         ><<><<<<<<<<    ><<
//  ><<       ><< ><<       ><<   ><<
//   ><<     ><<  ><<      ><<
//    ><<   ><<   ><<<<<<  ><<
//     ><< ><<    ><<      ><<
//      ><<<<     ><<       ><<   ><<
//       ><<      ><<<<<<<<   ><<<<


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

CaveError cave_vec_add_at_unchecked_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveVec v;
    cave_vec_init(&v, sizeof(size_t), 10000, &err);
    if(err != CAVE_NO_ERROR) {
        return err;
    }

    for(size_t i = 0; i < 10000; i++) {
        cave_vec_push(&v, &i, &err);
        if(err != CAVE_NO_ERROR) {
            return err;
        }
    }

    for(size_t i = 0; i < v.len; i++) {
        size_t element = *(size_t*)cave_vec_at_unchecked(&v, i);
        if(element != i) {
            return CAVE_DATA_ERROR;
        }
    }
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


//   ><<     ><<      ><         ><< <<   ><<     ><<
//   ><<     ><<     >< <<     ><<    ><< ><<     ><<
//   ><<     ><<    ><  ><<     ><<       ><<     ><<
//   ><<<<<< ><<   ><<   ><<      ><<     ><<<<<< ><<
//   ><<     ><<  ><<<<<< ><<        ><<  ><<     ><<
//   ><<     ><< ><<       ><< ><<    ><< ><<     ><<
//   ><<     ><<><<         ><<  ><< <<   ><<     ><<

// the purpose of this test is to just gather some data on how good the `rand()` call is
// so when doing statistical tests, like the following hash tests, we have a distribution to compare
// to. 
CaveError randomness_sanity_check(const char* rel_output_dir) {
    size_t total_size = 10000000;
    size_t buff_len = 100000;
    //total_size and buff_len are liable to get fiddled with, so feels prudent to add this check
    if(total_size % buff_len != 0) {return CAVE_INDEX_ERROR; }
    //Just noting that this is a VLA, which there is some reason to be ambivalent about
    int buffer[buff_len];

    char * out_file_name = cave_str_combine_unchecked(rel_output_dir, "/randomness_sanity_check.csv");
    FILE* out_file = fopen(out_file_name, "w");

    for(size_t i = 0; i < total_size / buff_len; i++) {
        for(size_t j = 0; j < buff_len; j++) {
            buffer[j] = rand();
        }
        for(size_t j = 0; j < buff_len; j++) {
            fprintf(out_file, "%i\n", buffer[j]);
        }
    }

    fprintf(out_file, "\n");

    fclose(out_file);
    free(out_file_name);

    return CAVE_NO_ERROR;
}

CaveError cave_hash_uint8_test(const char* rel_output_dir) {
    size_t total_size = 10000000;
    size_t buff_len = 100000;
    //total_size and buff_len are liable to get fiddled with, so feels prudent to add this check
    if(total_size % buff_len != 0) {return CAVE_INDEX_ERROR; }
    //Just noting that this is a VLA, which there is some reason to be ambivalent about
    uint8_t buffer[buff_len];

    char * out_file_name = cave_str_combine_unchecked(rel_output_dir, "/cave_hash_uint8_test.csv");
    FILE* out_file = fopen(out_file_name, "w");

    for(size_t i = 0; i < total_size / buff_len; i++) {
        for(size_t j = 0; j < buff_len; j++) {
            //ToDo: clang-tidy is yelling at me that rand has limited randomness.
            uint8_t x = (uint8_t)rand();
            buffer[j] = cave_hash_uint8(x);
        }
        for(size_t j = 0; j < buff_len; j++) {
            fprintf(out_file, "%" PRIu8 "\n", buffer[j]);
        }
    }

    fprintf(out_file, "\n");

    fclose(out_file);
    free(out_file_name);

    return CAVE_NO_ERROR;
}

CaveError cave_hash_uint16_test(const char* rel_output_dir) {
    size_t total_size = 10000000;
    size_t buff_len = 100000;
    //total_size and buff_len are liable to get fiddled with, so feels prudent to add this check
    if(total_size % buff_len != 0) {return CAVE_INDEX_ERROR; }
    //Just noting that this is a VLA, which there is some reason to be ambivalent about
    uint16_t buffer[buff_len];

    char * out_file_name = cave_str_combine_unchecked(rel_output_dir, "/cave_hash_uint16_test.csv");
    FILE* out_file = fopen(out_file_name, "w");

    for(size_t i = 0; i < total_size / buff_len; i++) {
        for(size_t j = 0; j < buff_len; j++) {
            //ToDo: clang-tidy is yelling at me that rand has limited randomness.
            uint16_t x = (uint16_t)rand();
            buffer[j] = cave_hash_uint16(x);
        }
        for(size_t j = 0; j < buff_len; j++) {
            fprintf(out_file, "%" PRIu16 "\n", buffer[j]);
        }
    }
    fprintf(out_file, "\n");

    fclose(out_file);
    free(out_file_name);

    return CAVE_NO_ERROR;
}

CaveError cave_hash_uint32_test(const char* rel_output_dir) {
    size_t total_size = 10000000;
    size_t buff_len = 100000;
    //total_size and buff_len are liable to get fiddled with, so feels prudent to add this check
    if(total_size % buff_len != 0) {return CAVE_INDEX_ERROR; }
    //Just noting that this is a VLA, which there is some reason to be ambivalent about
    uint32_t buffer[buff_len];

    char * out_file_name = cave_str_combine_unchecked(rel_output_dir, "/cave_hash_uint32_test.csv");
    FILE* out_file = fopen(out_file_name, "w");

    for(size_t i = 0; i < total_size / buff_len; i++) {
        for(size_t j = 0; j < buff_len; j++) {
            //ToDo: clang-tidy is yelling at me that rand has limited randomness.
            uint32_t x = (uint32_t)rand();
            buffer[j] = cave_hash_uint32(x);
        }
        for(size_t j = 0; j < buff_len; j++) {
            fprintf(out_file, "%" PRIu32 "\n", buffer[j]);
        }
    }
    fprintf(out_file, "\n");

    fclose(out_file);
    free(out_file_name);

    return CAVE_NO_ERROR;
}

CaveError cave_hash_uint64_test(const char* rel_output_dir) {
    size_t total_size = 10000000;
    size_t buff_len = 100000;
    //total_size and buff_len are liable to get fiddled with, so feels prudent to add this check
    if(total_size % buff_len != 0) {return CAVE_INDEX_ERROR; }
    //Just noting that this is a VLA, which there is some reason to be ambivalent about
    uint64_t buffer[buff_len];

    char * out_file_name = cave_str_combine_unchecked(rel_output_dir, "/cave_hash_uint64_test.csv");
    FILE* out_file = fopen(out_file_name, "w");

    for(size_t i = 0; i < total_size / buff_len; i++) {
        for(size_t j = 0; j < buff_len; j++) {
            //ToDo: clang-tidy is yelling at me that rand has limited randomness.
            //!IMPORTANT! rand returns an int which (almost certainly) is never gonna be 64 bits
            //together these two lines initalize 64 random bits
            uint64_t x = (uint64_t)rand();
            x = (x<<32) + (uint64_t)rand();
            buffer[j] = cave_hash_uint64(x);
        }
        for(size_t j = 0; j < buff_len; j++) {
            fprintf(out_file, "%" PRIu64 "\n", buffer[j]);
        }
    }
    fprintf(out_file, "\n");

    fclose(out_file);
    free(out_file_name);

    return CAVE_NO_ERROR;
}

CaveError cave_hash_bytes_test(const char* rel_output_dir) {
    return CAVE_DATA_ERROR;
}

CaveError cave_hash_str_test(const char* rel_output_dir) {
    return CAVE_DATA_ERROR;
}




//  ><<     ><<      ><         ><< <<  ><<     ><<    ><<       ><<      ><       ><<<<<<<
//  ><<     ><<     >< <<     ><<    ><<><<     ><<    >< ><<   ><<<     >< <<     ><<    ><<
//  ><<     ><<    ><  ><<     ><<      ><<     ><<    ><< ><< > ><<    ><  ><<    ><<    ><<
//  ><<<<<< ><<   ><<   ><<      ><<    ><<<<<< ><<    ><<  ><<  ><<   ><<   ><<   ><<<<<<<
//  ><<     ><<  ><<<<<< ><<        ><< ><<     ><<    ><<   ><  ><<  ><<<<<< ><<  ><<
//  ><<     ><< ><<       ><< ><<    ><<><<     ><<    ><<       ><< ><<       ><< ><<
//  ><<     ><<><<         ><<  ><< <<  ><<     ><<    ><<       ><<><<         ><<><<

CaveError cave_hashmp_init_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveHashMap h1;
    cave_hashmp_init(
            &h1,
            sizeof(uint32_t),
            sizeof(uint64_t),
            256,
            (CAVE_HASH_FN) &cave_idx_hash_uint32,
            &err
            );

    if(err != CAVE_NO_ERROR) {
        return CAVE_DATA_ERROR;
    }

    bool correct = true;
    correct &=
            h1.count == 0 &&
            h1.capacity >= 256 &&
            h1.key_size == sizeof(uint32_t) &&
            h1.value_size == sizeof(uint64_t) &&
            h1.hash_fn == (CAVE_HASH_FN)&cave_idx_hash_uint32 &&
            h1.key_eq_fn == NULL &&
            h1.key_cpy_fn == NULL &&
            h1.value_cpy_fn == NULL &&
            h1.kv_destructor == NULL;

    correct &=
            h1.buckets.len == h1.capacity &&
            h1.buckets.stride == sizeof(CaveVec);

    if(!correct) {
        return CAVE_DATA_ERROR;
    }
    //TODO: test more edge cases, eg, 0's and NULL's
    return CAVE_NO_ERROR;
}

CaveError cave_hashmp_insert_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveHashMap h1;
    cave_hashmp_init(
            &h1,
            sizeof(uint32_t),
            sizeof(uint64_t),
            256,
            (CAVE_HASH_FN) &cave_idx_hash_uint32,
            &err
    );

    for(uint32_t i = 0; i < 1000; i++) {
        uint64_t value = (uint64_t)(i) * 2;
        cave_hashmp_insert(&h1, &i, &value, &err);
        if(err != CAVE_NO_ERROR) {
            return err;
        }
    }

    if(h1.count != 1000) {
        return CAVE_DATA_ERROR;
    }

    size_t total_elements = 0;
    for(size_t i = 0; i < h1.buckets.len; i++) {
        CaveVec* bucket = cave_vec_at_unchecked(&h1.buckets, i);
        if(bucket->len > 0) {
            total_elements += bucket->len;
            if(err != CAVE_NO_ERROR) {return CAVE_INSUFFICIENT_MEMORY_ERROR;}
        }
    }

    if(total_elements != h1.count) {
        return CAVE_COUNT_ERROR;
    }

    //ToDo: test this wayyyy more thoroughly

    return CAVE_NO_ERROR;
}

CaveError cave_hashmp_update_or_insert_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_at_test() {
    CaveError err = CAVE_NO_ERROR;
    CaveHashMap h1;
    cave_hashmp_init(
        &h1,
        sizeof(size_t),
        sizeof(size_t),
        25,
        (CAVE_HASH_FN)&cave_idx_hash_sizet,
        &err
    );
    if(err != CAVE_NO_ERROR) { return err; }

    for(size_t i = 0; i < 10000; i++) {
        cave_hashmp_insert(&h1, &i, &i, &err);
        if(err != CAVE_NO_ERROR) { return err; }
    }

    for(size_t i = 0; i < 10000; i++) {
        size_t* value = cave_hashmp_at(&h1, &i, &err);
        if(err != CAVE_NO_ERROR) { return err; }
        if(*value != i) { return CAVE_DATA_ERROR; }
    }

    // So far so good :)
    // Now lets try it on a more complicated type.

    //ToDo: make it easier to declare a HashMap with strings
    CaveHashMap str_map;
    cave_hashmp_init(
        &str_map,
        sizeof(char*),
        sizeof(char*),
        30,
        (CAVE_HASH_FN)&cave_idx_hash_str,
        &err
    );
    if(err != CAVE_NO_ERROR) { return err; }
    cave_hashmp_set_key_eq_fn(&str_map, (CAVE_KEY_EQ_FN)&cave_kv_str_key_eq);
    cave_hashmp_set_key_cpy_fn(&str_map, (CAVE_KEY_CPY_FN)&cave_kv_str_key_cpy);
    cave_hashmp_set_value_cpy_fn(&str_map, (CAVE_VALUE_CPY_FN)&cave_kv_str_value_cpy);
    cave_hashmp_set_kv_destructor_fn(&str_map, (CAVE_KV_DESTRUCT_FN)&cave_kv_str_str_destruct);


    size_t kv_count = 7;
    char const * keys[] = {
        "John", "Stacy", "Bob", "Tammera", "Joseph", "James", "Alexis", 
    };
    char const * values[] = {
        "Smith", "Jones", "Ogden", "Trilby", "Connor", "Hammond", "Wolner"
    };

    for(size_t i = 0; i < kv_count; i++) {
        char const * key = keys[i];
        char const * value = values[i];
        cave_hashmp_insert(&str_map, &key, &value, &err);
        if(err != CAVE_NO_ERROR) { return err; }
    }


//iterating over all keys, but going backwards just to mix it up. Never know when 
//that kind of thing could matter, which is the point of these tests. 
    for(size_t i = kv_count; i-- > 0; ) {
        char const * key = keys[i];
        char const * value = values[i];

        char ** ret_value = cave_hashmp_at(&str_map, &key, &err);
        if(err != CAVE_NO_ERROR) { return err; }

        int str_comparision = strcmp(value, *ret_value);
        if(str_comparision != 0) { return CAVE_DATA_ERROR; }

        //this would imply that when the value was inserted into str_map, rather than
        // doing a deep copy, just copied the pointer, which should not be the behavior of 
        // str_map given the copy functions we set. So error.
        if(*ret_value == value) { return CAVE_DATA_ERROR; }
    }

    //gonna double check that we are doing actual string comparision and not just pointer comparision
    char const *  dup_key = "Joseph";
    char ** ret_value = cave_hashmp_at(&str_map, &dup_key, &err);
    if(err != CAVE_NO_ERROR) { return err; }
    int str_comparision = strcmp(values[4], *ret_value);
    if(str_comparision != 0) { return CAVE_DATA_ERROR; }


    //if we get here, YAY. Now lets test that the correct errors are returned when something is wrong.

    // h is NULL
    void * ret = cave_hashmp_at(NULL, keys[0], &err);
    if(err != CAVE_INVALID_ARGUMENT_ERROR) { return CAVE_INVALID_ARGUMENT_ERROR ;}
    if(ret != NULL) {return CAVE_DATA_ERROR;}
    // key is NULL
    ret = cave_hashmp_at(&str_map, NULL, &err);
    if(err != CAVE_INVALID_ARGUMENT_ERROR) { return CAVE_INVALID_ARGUMENT_ERROR ;}
    if(ret != NULL) {return CAVE_DATA_ERROR;}

    // key not in str_map
    char const * not_key = "lemon";
    ret = cave_hashmp_at(&str_map, &not_key, &err);
    if(err != CAVE_MISSING_KEY_ERROR) { return CAVE_MISSING_KEY_ERROR;}
    if(ret != NULL) {return CAVE_DATA_ERROR;}

    //thats more or less everything. just cleanup now.
    cave_hashmp_release(&h1);
    cave_hashmp_release(&str_map);

    return CAVE_NO_ERROR;
}

CaveError cave_hashmp_remove_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_cpy_kv_into_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_move_kv_into_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_clear_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_cpy_collect_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_mv_collect_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_rehash_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_total_collisions_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_max_collisions_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_cpy_assign_test() {
    return CAVE_DATA_ERROR;
}

CaveError cave_hashmp_cpy_init_test() {
    return CAVE_DATA_ERROR;
}


void uppercase_kv_str_str(CaveKeyValue* kv, size_t* count, CaveError* err) {
    char* key = *(char**)kv->key;
    char* value = *(char**)kv->value;
    size_t key_len = strlen(key);
    size_t value_len = strlen(value);

    for(size_t i = 0; i < key_len; i++) {
        key[i] = (char)toupper(key[i]);
    }

    for(size_t i = 0; i < value_len; i++) {
        value[i] = (char)toupper(value[i]);
    }

    *count += 1;
    *err = CAVE_NO_ERROR;
}

CaveError cave_hashmp_foreach_test() {
    CaveError err = CAVE_NO_ERROR;

    CaveHashMap h1; // 
    cave_hashmp_init(
        &h1,
        sizeof(char*),
        sizeof(char*),
        30,
        (CAVE_HASH_FN)cave_idx_hash_str,
        &err
    );

    cave_hashmp_set_key_cpy_fn(&h1, (CAVE_KEY_CPY_FN)cave_kv_str_key_cpy);
    cave_hashmp_set_value_cpy_fn(&h1, (CAVE_VALUE_CPY_FN)cave_kv_str_value_cpy);
    cave_hashmp_set_key_eq_fn(&h1, (CAVE_KEY_EQ_FN)cave_kv_str_key_eq);
    //ToDO: write a test destructor to properly ensure this is always called correctly? 
    cave_hashmp_set_kv_destructor_fn(&h1, (CAVE_KV_DESTRUCT_FN)cave_kv_str_str_destruct); 

    const size_t kv_count = 5;
    char const * keys[] = {
        "apple", "orange", "lemon", "avocado", "peach"
    };

    char const * values[] = {
        "red", "orange", "yellow", "green", "pink"
    };

    for(size_t i = 0; i < kv_count; i++) {
        cave_hashmp_insert(&h1, &keys[i], &values[i], &err);
        if(err != CAVE_NO_ERROR) {  return err; }
    }

    size_t count = 0;
    cave_hashmp_foreach(
        &h1, 
        (CAVE_FOREACH_CLOSURE)uppercase_kv_str_str,
        &count,
        &err
        );

    //every kv-pair should now be uppercased, and count should have counted the number of kv-pairs

    if(count != kv_count) { return CAVE_COUNT_ERROR;}

    CaveVec h1_collected = cave_hashmp_cpy_collect(&h1, &err);
    if(err != CAVE_NO_ERROR ) {  return err; }
    if(h1_collected.len != kv_count) { return CAVE_COUNT_ERROR; }

    char const * keys_upper[] = {
        "APPLE", "ORANGE", "LEMON", "AVOCADO", "PEACH"
    };

    char const * values_upper[] = {
        "RED", "ORANGE", "YELLOW", "GREEN", "PINK"
    };


    for(size_t i = 0; i < kv_count; i++) {
        char const * upper_key = keys_upper[i];
        char const * upper_value = values_upper[i];

        bool match_found = false;
        for(size_t j = 0; j < h1_collected.len; j++) {
            CaveKeyValue* kv = cave_vec_at_unchecked(&h1_collected, j);
            char* h1_key = *(char**)kv->key;
            char* h1_value = *(char**)kv->value;

            int keys_eq = strcmp(upper_key, h1_key);
            int vals_eq = strcmp(upper_value, h1_value);
            if(keys_eq == 0 && vals_eq == 0) {
                match_found = true;
                break;
            }
        }
        if(!match_found) {
            return CAVE_DATA_ERROR;
        }
    }

    //TODO (cleanup): release all memory.
    cave_hashmp_release(&h1);
    cave_vec_release(&h1_collected);

    return CAVE_NO_ERROR;
}




int main(int argc, char* argv[]) {

//initialize things the rest of the tests will use

    //may spend the effort to customize this more later, but feels like
    //overkill atm.
    char const * out_dir = "./";

    //time_t's are so annoying.
    srand(time(NULL));

    int test_fails = 0;

//Vec Tests
    RUN_TEST(cave_vec_init_test,                test_fails);
    RUN_TEST(cave_vec_reserve_test,             test_fails);
    RUN_TEST(cave_vec_shrink_test,              test_fails);
    RUN_TEST(cave_vec_push_test,                test_fails);
    RUN_TEST(cave_vec_at_test,                  test_fails);
    RUN_TEST(cave_vec_add_at_unchecked_test,    test_fails);
    RUN_TEST(cave_vec_add_at_test,              test_fails);
    RUN_TEST(cave_vec_end_test,                 test_fails);
    RUN_TEST(cave_vec_pop_test,                 test_fails);
    RUN_TEST(cave_vec_remove_at_test,           test_fails);
    RUN_TEST(cave_vec_cpy_init_test,            test_fails);
    RUN_TEST(cave_vec_cpy_assign_test,          test_fails);
    RUN_TEST(cave_vec_clear_test,               test_fails);
    RUN_TEST(cave_vec_foreach_test,             test_fails);
    RUN_TEST(cave_vec_filter_test,              test_fails);
    RUN_TEST(cave_vec_map_test,                 test_fails);


//just commented out while writing other tests cuz they take a long time. 
// ToDo: Multithread running tests?

//Hash Tests
    RUN_OUTPUT_TEST(randomness_sanity_check,    test_fails,  out_dir);
    RUN_OUTPUT_TEST(cave_hash_uint8_test,       test_fails,  out_dir);
    RUN_OUTPUT_TEST(cave_hash_uint16_test,      test_fails,  out_dir);
    RUN_OUTPUT_TEST(cave_hash_uint32_test,      test_fails,  out_dir);
    RUN_OUTPUT_TEST(cave_hash_uint64_test,      test_fails,  out_dir);
    RUN_OUTPUT_TEST(cave_hash_bytes_test,       test_fails,  out_dir);
    RUN_OUTPUT_TEST(cave_hash_str_test,         test_fails,  out_dir);

//HashMap Tests
    RUN_TEST(cave_hashmp_init_test,             test_fails);
    RUN_TEST(cave_hashmp_insert_test,           test_fails);
    RUN_TEST(cave_hashmp_update_or_insert_test, test_fails);
    RUN_TEST(cave_hashmp_at_test,               test_fails);
    RUN_TEST(cave_hashmp_remove_test,           test_fails);
    RUN_TEST(cave_hashmp_cpy_kv_into_test,      test_fails);
    RUN_TEST(cave_hashmp_move_kv_into_test,     test_fails);
    RUN_TEST(cave_hashmp_clear_test,            test_fails);
    RUN_TEST(cave_hashmp_cpy_collect_test,      test_fails);
    RUN_TEST(cave_hashmp_mv_collect_test,       test_fails);
    RUN_TEST(cave_hashmp_rehash_test,           test_fails);
    RUN_TEST(cave_hashmp_total_collisions_test, test_fails);
    RUN_TEST(cave_hashmp_max_collisions_test,   test_fails);
    RUN_TEST(cave_hashmp_cpy_assign_test,       test_fails);
    RUN_TEST(cave_hashmp_cpy_init_test,         test_fails);
    RUN_TEST(cave_hashmp_foreach_test,          test_fails);

    return test_fails;
}