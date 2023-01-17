//
// Created by David Sullivan on 11/23/22.
//

#ifndef CAVE_BEDROCK_H
#define CAVE_BEDROCK_H

#include <stddef.h>
#include <stdbool.h>
#include "cave-error.h"

/// \file
/// Bedrock is a sublibrary of Cave. In particular, Bedrock provides foundational
/// data-structures for the rest of Cave to use. Currently this just consists of
/// a vector data-structure, but at least a hashmap implementation will be added,
/// and whatever other foundational data-structures other Cave libraries will need.
///
/// NOTE: The majority of functions here accept NULL arguments in most of their paramaters.
/// However, in none  of the following functions is it valid to pass NULL as the error
/// argument. Passing NULL as the error argument will end up in undefined behavior via
/// trying to write a value to NULL.



/// The factor by which a vector's allocation will be grown every time it's grown.
#define CAVE_VEC_GROW_FACTOR (2)
/// The default capacity for initializing a vector
#define CAVE_VEC_DEFAULT_CAPACITY (256)



/// A simple runtime-generic dynamically resizeable array struct, ie a "vector".
/// In other words, represents a contiguous list of elements of the same size, which is set at runtime.
/// This list is not a set size, and will grow as necessary as items are added to it.
///
/// It is generally valid to read from and write to the memory pointed to between
/// `data` and `data + (element_size * len)`. However, because many `cave_vec` functions reallocate,
/// pointers to any of that data may become invalidated across `cave_vec` function calls.
///
/// When the vector is no longer needed, call `cave_vec_release()` on it to free the memory.
///
/// `data`, `element_size`, `capacity` and `len` should never be modified directly.
typedef struct CaveVec {
    void* data;
    size_t stride;
    size_t capacity;
    size_t len;
} CaveVec;


/// \brief Initializes `v`.
///
/// The majority of operations on a vector are not valid until `cave_vec_init` is called on it.
///
/// \param v - The vector to initialize.
/// \param element_size - The number of bytes an element takes in memory. eg, if storing `int`s,
///                you would pass `sizeof(int)` as element size. An element size of 0 will return an error.
/// \param initial_capacity - The number of elements for `v` to be able to store before having to reallocate.
///                    If `initial_capacity` is set to 0, then it is like it was called with `CAVE_VEC_DEFAULT_CAPACITY`
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL or `element_size` is zero.
///                   * CAVE_INSUFFICIENT_MEMORY_ERROR - If malloc'ing does not succeed.
/// \returns `v` if successful, and `NULL` if there is an error.
CaveVec* cave_vec_init(CaveVec* v, size_t element_size, size_t initial_capacity, CaveError* err);

/// \brief Grows or shrinks `v` such that it has the ability to store `capacity` number of objects.
/// Though will never shrink below the ability to hold `v->len` number of objects.
///
/// As long as `capacity` is greater than or equal to `v->len`, attempts to reallocate `v->data` based on
/// the number of elements specified by `capacity`. To be clear, this function does not change the length
/// of `v`, just when it will next need to reallocate as elements are pushed into it.
///
/// NOTE: Does not check that `capacity` is greater than 0! (Perhaps it should.)
///
/// \param v - The target vector to act on.
/// \param capacity - The number of objects for `v` to be able to hold.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL or `capacity` is less than `v->len`.
///                   * CAVE_INSUFFICIENT_MEMORY_ERROR - If unable to realloc with the requested capacity.
/// \returns `v` if successful, and `NULL` if there is an error.
CaveVec* cave_vec_reserve(CaveVec* v, size_t capacity, CaveError* err);

/// \brief Reduces the allocation held by `v-data` to the smallest it can be.
///
/// Equivalent to calling `cave_vec_reserve(v, v->len, err)`.
///
/// \param v - The target vector.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
/// \return `v` on success and NULL on error.
CaveVec* cave_vec_shrink(CaveVec* v, CaveError* err);

/// \brief Frees the memory held by `v`. Should always be called at the end of v's lifetime.
///
/// \param v - The target vector.
void cave_vec_release(CaveVec* v);

/// \brief Copies `element` onto the end to the `v->data`, reallocating if necessary, and increasing
/// `v->len` by 1.
///
/// \param v - Target vector to act on.
/// \param element - A pointer to the object to copy into `v->data`. `v->element_size` number of bytes will be
///                  copied.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL or `element` is NULL.
///                   * CAVE_INSUFFICIENT_MEMORY_ERROR - If `v` needs to realloc to accommodate another element,
///                   but the reallocation is unsuccessful.
/// \returns `v` if successful, and `NULL` if there is an error.
CaveVec* cave_vec_push(CaveVec* v, void const* element, CaveError* err);

/// \brief The primary way to access and modify an element of the vector.
///
/// \param v - The target vector.
/// \param index - The index of the object to retrieve. Must be between 0 and `v->len - 1` (inclusive).
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
///                   * CAVE_INDEX_ERROR - If index is greater than `v->len - 1`.
/// \return A pointer to the element at `index` on success, or NULL if there is an error.
/// Strictly speaking, if all parameters are valid, then
/// the pointer returned is `v->data + (v->element_size * index)`. You may read from and write to this pointer.
/// However, as many `cave_vec` functions reallocate, this pointer may become invalidated across
/// a `cave_vec` function call.
void* cave_vec_at(CaveVec const* v, size_t index, CaveError* err);

/// \brief The way to access and modify an element of the vector without bounds checking.
///
/// Obviously memory issues abound if index is greater than `v->len`.
inline void* cave_vec_at_unchecked(CaveVec* v, size_t index);

/// \brief Copies `element` into the index specified.
///
/// Shifts all the elements at or after index over by 1, reallocating if necessary,
/// then copies the object pointed at by `element` into `v` at the position specified by
/// `index`.
///
/// \param v - The target vector.
/// \param element - Pointer to the object to copy into `v`. `v->element_size` number of bytes
///                  will be copied from `element` into `v`.
/// \param index - The index of the where to copy the element. Must be between 0 and `v->len - 1` (inclusive).
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
///                   * CAVE_INDEX_ERROR - If index is greater than `v->len - 1`.
/// \return `v` if successful, NULL if there is an error.
CaveVec* cave_vec_add_at(CaveVec* v, void const* element, size_t index, CaveError* err);

/// \brief Pointer to last element stored in `v`.
///
/// Literally equivalent to `cave_vec_at(v, v->len - 1, err);`
///
/// \param v - The target vertex.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
///                   * CAVE_INDEX_ERROR - If index is greater than `v->len - 1`.
/// \return A pointer to the last element held by `v` on success, or NULL if there is an error.
/// Just as in a call to `cave_vec_at()`, you may read from and write to this pointer.
///// However, as many `cave_vec` functions reallocate, this pointer may become invalidated across
///// a `cave_vec` function call.
void* cave_vec_end(CaveVec const* v, CaveError* err);

/// \brief Removes last element from `v` and optionally copies it into `dest`.
///
/// If `dest` is NULL, then just reduces `v->len` by 1. If `dest` is not NULL, then
/// the last element stored in `v` is copied into `dest`, and then reduces `v->len` by 1.
///
/// \param v - The target vector.
/// \param dest - Pointer to where the last element of `v` will be copied to.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
///                   * CAVE_INDEX_ERROR - if `v->len == 0`.
/// \return `dest` on success (which may be NULL) and NULL on failure.
void* cave_vec_pop(CaveVec* v, void* dest, CaveError* err);

/// \brief Removes element from `v` at `index`, optionally copying it into `dest`.
///
/// Removes the element at `index` from `v`, and shifts all elements past `index` back
/// one to fill in the gap. If `dest` is not NULL, then the element will first be copied into
/// `dest`. If NULL, then the element is just removed and not copied.
///
/// \param v - The target vector.
/// \param dest - Optional address of where to copy the removed element to (can be NULL).
/// \param index - The index of the element to remove.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
///                   * CAVE_INDEX_ERROR - If index is greater than `v->len - 1`.
/// \return `v` on success, and NULL if there is an error.
CaveVec* cave_vec_remove_at(CaveVec* v, void* dest, size_t index, CaveError* err);

/// \brief Empties a vector of elements but leaves it initialized.
///
/// Really all this function does is set `v->len` to be 0. But that has the effect of
/// making the vector have no elements.
///
/// \param v - The target vector.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL.
/// \return
CaveVec* cave_vec_clear(CaveVec* v, CaveError* err);

/// \brief Initializes a vector as a copy of another vector
///
/// This function takes an uninitialized vector, initializes it, and bitwise-copies `src->data` into `dest->data`.
/// `src` must be already initialized and `dest` must NOT be already initialized.
/// If `dest` is already initialized, that violates this function's contract.
/// However, if `cave_vec_release` is first called on `dest`, then it is valid to call this function on dest.
/// If there is an error, then `dest` is not initialized.
///
/// NOTE: violating this functions contract with a pre-initialized `dest` vector will likely result in
/// a memory leak.
///
/// \param dest - Vector to initialize and copy into.
/// \param src - Vector to copy from.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `dest` is NULL or `src` is NULL.
///                   * CAVE_INSUFFICIENT_MEMORY_ERROR - If there is not enough memory to create a copy of
///                   `src->data`.
/// \return `dest` if there is no error, and NULL if there is an error.
CaveVec* cave_vec_cpy_init(CaveVec* dest, CaveVec const* src, CaveError* err);

/// \brief Copies a vector into another initialized vector, discarding its prior contents.
///
/// This function takes an initialized vector, `src`, and bitwise copies the contents of `src->data`
/// into `dest->data`, `dest` also being an initialized vector. The contents of `dest->data` are
/// discarded but not necessarily erased. `dest->data` may also be reallocated, so it is not
/// valid to hold a variable set to `dest->data` across this function call.
///
/// \param dest - The vector to be copied into.
/// \param src - The vector to be copied from.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `dest` is NULL or `src` is NULL.
///                   * CAVE_INSUFFICIENT_MEMORY_ERROR - If there is not enough memory to create a copy of
///                   `src->data`.
/// \return `dest` on success and NULL if there is an error.
CaveVec* cave_vec_cpy_assign(CaveVec* dest, CaveVec const* src, CaveError* err);

/// Copies all the elements from `src` into the end of `dest`
///
/// \param dest - The CaveVec being copied into.
/// \param src - The CaveVec being copied from.
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_INSUFFICIENT_MEMORY_ERROR - If there is not enough memory to copy the contents
///                   of `src` to `dest`.
/// \return `dest` on success, `NULL` on error.
CaveVec* cave_vec_append(CaveVec* dest, CaveVec const* src, CaveError* err);


typedef void (*CAVE_FOREACH_CLOSURE)(void* element, void* closure_data, CaveError* err);
typedef bool (*CAVE_FILTER_CLOSURE)(void const* element, void* closure_data, CaveError* err);
typedef void (*CAVE_MAP_CLOSURE)(void const* input_elm, void* output_elm, void* closure_data, CaveError* err);

/// \brief Takes a closure, and applies it in place to every element in `v` in sequence.
///
/// This function iterates through every element in `v` from first to last, and passes the
/// address to the element as the first parameter to `fn`. For each iteration, `closure_data`
/// is passed as the second argument to `fn`. Additionally, an error argument is passed to `fn`,
/// and if at any point that error argument is not `CAVE_NO_ERROR`, then iteration stops and
/// the value of `err` is set to the the error set by `fn`, and NULL is returned. `fn` may
/// modify both the element passed to it, as well as `closure_data` however the user wishes.
///
/// \param v - The target vector.
/// \param fn - The closure that gets applied to each element.
/// \param closure_data - Parameter that gets passed as second argument to each invocation  of `fn`.
///                       (may be NULL).
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL or `fn` is NULL.
///                   * any error that is set by `fn`.
/// \return `v` on success, NULL if an error is encountered.
CaveVec* cave_vec_foreach(CaveVec* v, CAVE_FOREACH_CLOSURE fn, void* closure_data, CaveError* err);

/// \brief Takes a closure, applies it to every element in `v` in sequence, and if the closure
/// returns true for that element, it is kept. Otherwise it is removed.
///
/// This function iterates through every element in `v` from first to last, and passes the
/// address to the element as the first parameter to `fn`. For each iteration, `closure_data`
/// is passed as the second argument to `fn`. Additionally, an error argument is passed to `fn`,
/// and if at any point that error argument is not `CAVE_NO_ERROR`, then iteration stops and
/// the value of `err` is set to the the error set by `fn`, and NULL is returned. Unlike
/// cave_vec_foreach(), `element` in `fn` is not modifiable. However, `closure_data` is, and
/// it is totally fine to modify that however the user desires.
///
/// \param v - The target vector.
/// \param fn - The closure that gets applied to each element.
/// \param closure_data - Parameter that gets passed as second argument to each invocation  of `fn`.
///                       (may be NULL).
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL or `fn` is NULL.
///                   * any error that is set by `fn`.
/// \return `v` on success, NULL if an error is encountered.
CaveVec* cave_vec_filter(CaveVec* v, CAVE_FILTER_CLOSURE, void* closure_data, CaveError* err);

/// \brief Takes a closure, applies it to every element of `src` in sequence, and fills
/// dest with the output from the closure, which is potentially a different type.
///
/// `dest` MUST be uninitialized, as this function will initialize it and fill it.
///
/// This function iterates through every element in `src` from first to last, and passes the
/// address to the element as the first parameter to `fn`. The second argument passed to `fn` is the
/// address of where the corresponding output element is located in `dest`. For each iteration,
/// `closure_data` is passed as the third argument to `fn`. Additionally, an error argument is passed to `fn`,
/// and if at any point that error argument is not `CAVE_NO_ERROR`, then iteration stops and
/// the value of `err` is set to the the error set by `fn`, and NULL is returned.
///
/// \param dest - Pointer to the uninitialized vector that will hold the output of every call to `fn`.
/// \param src - Pointer to the vector that will have its elements iterated over.
/// \param output_stride - The size in bytes of the output element.
/// \param fn - The closure that gets applied to each element.
/// \param closure_data - Parameter that gets passed as second argument to each invocation  of `fn`.
///                       (may be NULL).
/// \param[out] err - The error recording argument. If there is an error, it is written to this argument.
///                   Otherwise `CAVE_NO_ERROR` is written to err.
///                   Errors:
///                   * CAVE_DATA_ERROR - If `v` is NULL or `fn` is NULL.
///                   * any error that is set by `fn`.
/// \return `v` on success, NULL if an error is encountered.
CaveVec* cave_vec_map(CaveVec* dest, CaveVec const * src, size_t output_stride, CAVE_MAP_CLOSURE fn, void* closure_data, CaveError* err);



/*
 * needs:
 * append
 * split_at
 * split_by
 * remove_indexes
 */


//   ><<     ><<      ><         ><< <<   ><<     ><<    ><<       ><<      ><       ><<<<<<<
//   ><<     ><<     >< <<     ><<    ><< ><<     ><<    >< ><<   ><<<     >< <<     ><<    ><<
//   ><<     ><<    ><  ><<     ><<       ><<     ><<    ><< ><< > ><<    ><  ><<    ><<    ><<
//   ><<<<<< ><<   ><<   ><<      ><<     ><<<<<< ><<    ><<  ><<  ><<   ><<   ><<   ><<<<<<<
//   ><<     ><<  ><<<<<< ><<        ><<  ><<     ><<    ><<   ><  ><<  ><<<<<< ><<  ><<
//   ><<     ><< ><<       ><< ><<    ><< ><<     ><<    ><<       ><< ><<       ><< ><<
//   ><<     ><<><<         ><<  ><< <<   ><<     ><<    ><<       ><<><<         ><<><<


typedef struct CaveKeyValue {
    void* key;
    void* value;
} CaveKeyValue;

size_t cave_hash_arbirary_bytes(void const * key, size_t byte_count);

typedef size_t (*CAVE_HASH_FN)(void const * key);

/// Interface for users to define equality for CaveHashMap keys. A `CAVE_KEY_EQ_FN` should return true
/// if the two keys are "equal" and false if they are "not equal". Useful for things like using
/// case insensitive strings as keys. **IMPORTANT** In order for a function to be a valid `CAVE_KEY_EQ_FN`,
/// the function should not return true unless the hash of key_a == hash of key_b . Obviously this depends on
// the hash function supplied. The bugs that are caused by this condition being violated are sporaddic and will
/// appear and dissapear.
typedef bool (*CAVE_KEY_EQ_FN)(void const * key_a, void const * key_b);

/// Can return an error for any reason, but generally should stick to returning an error on allocation failure.
typedef CaveError (*CAVE_KV_CPY_FN)(CaveKeyValue* dest, CaveKeyValue* src);

typedef void (*CAVE_KV_DESTRUCTOR)(CaveKeyValue kv);

//#define DEF_CAVE_HASH_FN_DEFAULT(key_type) \
//    #ifndef DEF_CAVE_HASH_FN_DEFAULT_#keytype \
//        #define DEF_CAVE_HASH_FN_DEFAULT_#keytype \
//        size_t cave_hash_fn_default_#keytype(void const * key) { \
//            return cave_hash_arbitrary_bytes(key, sizeof(key_type)); \
//        } \
//    #endif
//
//
//#define CAVE_HASH_FN_DEFAULT(key_type) \
//    #ifdef DEF_CAVE_HASH_FN_DEFAULT_#keytype \
//        (CAVE_HASH_FN)&cave_hash_fn_default_#keytype \
//    #else \
//        You must call the appropreate DEF_CAVE_HASH_FN_DEFAULT macro before using this one \
//
//
//
//#define DEF_CAVE_KEY_EQ_FN_DEFAULT(key_type) \
//
//#define DEF_CAVE_KV_CPY_DEFAULT(key_type, value_type) \
//
//#define CAVE_KEY_EQ_FN_DEFAULT(key_type) \

//#define CAVE_KV_CPY_DEFAULT(key_type, value_type) \



extern CAVE_KV_DESTRUCTOR cave_kv_default_destructor;

typedef struct CaveHashMap {
    size_t key_size;
    size_t value_size;
    size_t capacity;
    size_t count;
    CAVE_HASH_FN hash_fn;
    CAVE_KEY_EQ_FN key_eq_fn;
    CaveVec buckets; //this will be a Vec<Vec<CaveKeyValue>>
    CaveVec occupied_buckets; //list of indexes of buckets that are occupied. ie, all i such that buckets[i].len != 0.
} CaveHashMap;


CaveHashMap* cave_hashmp_init(
        CaveHashMap* h,
        size_t value_size,
        size_t key_size,
        size_t min_capacity,
        CAVE_HASH_FN hash_fn,
        CAVE_KEY_EQ_FN key_eq_fn,
        CaveError* err
        );

/// \brief Frees all memory `h` is holding, but does not call a destructor on the CaveKeyValue pairs as they are freed.
///
/// \param h - Initialized and valid CaveHashMap. May be empty, but no prior release function may have been applied to `h`.
void cave_hashmap_release_shallow(CaveHashMap* h);

/// \brief Calls destructor on every CaveKeyValue held by `h`, then frees all memory `h` is holding.
///
/// \param h - Initialized and valid CaveHashMap. May be empty, but no prior release function may have been applied to `h`.
void cave_hashmp_release_deep(CaveHashMap* h, CAVE_KV_DESTRUCTOR destructor_fn);

/// Creates and inserts
CaveHashMap* cave_hashmp_insert_(CaveHashMap* h, void const * key, void const * value, CaveError* err);

CavehashMap* cave_hashmp_insert_deep(
        CaveHashMap* h,
        void const * key,
        void const * value,
        CAVE_KV_CPY_FN,
        CaveError* err)

/// \brief Returns pointer to value associated with `key`
///
/// Finds a CaveKeyValue with a key that returns true when passed to `h->key_eq_fn`, and returns the value field.
/// If no such CaveKeyValue can be found, return null.
///
/// \param h
/// \param key
/// \param err
/// \return
void* cave_hashmp_at(CaveHashMap* h, void const * key, CaveError* err);

/// \return the key-value pair that was removed by this key
CaveKeyValue cave_hashmap_remove(CaveHashMap* h, void const * key, CaveError* err);

/// \brief Bitwise copies all data pointed to by the key-value pairs into a CaveVec.
void cave_hashmp_collect_shallow(CaveHashMap* h, CaveVec* v, CaveError* err);
/// \brief uses `cpy_fn` on every key-value pair in `h` to copy every key-value pair into `v`.
void cave_hashmp_collect_deep(CaveHashMap* h, CaveVec* v, CAVE_KV_CPY_FN cpy_fn, CaveError* err);

/// \brief Calls destructor on and removes every CaveKeyValue element in `h`, but does not free `h`'s memory.
CaveHashMap* cave_hashmp_clear(CaveHashMap* h, CaveError* err);

/// \brief Removes every CaveKeyValue element in `h`, but does not call the destructor on the elements, or free `h`'s memory.
CaveHashMap* cave_hashmp_clear_shallow(CaveHashMap* h, CaveError* err);

CaveHashMap* cave_hashmp_rehash(CaveHashMap* h, size_t new_min_capacity, CaveError* err);

size_t cave_hashmp_total_collisions(CaveHashMap* h);

size_t cave_hashmp_max_collisions(CaveHashMap* h );

//the following two functions perform shallow copies. If the keys or values being stored in the CaveHashMap are
//pointers to an item allocated on the heap, or needs some custom copy function to be run, call the _custom versions
//of these functions.
CaveHashMap* cave_hashmp_cpy_assign_shallow(CaveHashMap* dest, CaveHashMap const * src, CaveError* err);
CaveHashMap* cave_hashmp_cpy_assign_deep(CaveHashMap* dest, CaveHashMap const * src, CAVE_KV_CPY_FN cpy_fn, CaveError* err);

CaveHashMap* cave_hashmp_cpy_init_shallow(CaveHashMap* dest, CaveHashMap const * src, CaveError* err);
CaveHashMap* cave_hashmp_cpy_init_deep(CaveHashMap* dest, CaveHashMap const * src, CAVE_KV_CPY_FN cpy_fn, CaveError* err);


CaveHashMap* cave_hashmp_foreach(CaveHashMap * h, CAVE_FOREACH_CLOSURE fn, void* closure_data, CaveError* err);


#endif //CAVE_BEDROCK_H
