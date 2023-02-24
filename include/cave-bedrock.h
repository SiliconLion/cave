//
// Created by David Sullivan on 11/23/22.
//

#ifndef CAVE_BEDROCK_H
#define CAVE_BEDROCK_H

#include "cave-platform-info.h"

#include <stddef.h>
#include <stdint.h>
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
///                   * CAVE_INDEX_ERROR - If `v` has no elements.
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


//   ><<     ><<      ><         ><< <<   ><<     ><<
//   ><<     ><<     >< <<     ><<    ><< ><<     ><<
//   ><<     ><<    ><  ><<     ><<       ><<     ><<
//   ><<<<<< ><<   ><<   ><<      ><<     ><<<<<< ><<
//   ><<     ><<  ><<<<<< ><<        ><<  ><<     ><<
//   ><<     ><< ><<       ><< ><<    ><< ><<     ><<
//   ><<     ><<><<         ><<  ><< <<   ><<     ><<

/// \defgroup CaveHash CaveHash
///@{

/// `cave_hash_<type>` functions will always have a return value of `<type>`,
/// whereas `cave_idx_hash_<type>` will always have a return type of `size_t`, and
/// have a type signature that is easily cast-able to `CAVE_HASH_FN`.

uint8_t  cave_hash_uint8(uint8_t x);
uint16_t cave_hash_uint16(uint16_t x);
uint32_t cave_hash_uint32(uint32_t x);
uint64_t cave_hash_uint64(uint64_t x);
size_t   cave_hash_sizet(size_t x);

size_t cave_idx_hash_uint8(uint8_t const * x);
size_t cave_idx_hash_uint16(uint16_t const * x);
size_t cave_idx_hash_uint32(uint32_t const * x);
size_t cave_idx_hash_uint64(uint64_t const * x);
size_t cave_idx_hash_sizet(size_t const * x);



/// \brief Takes an arbitrary number of bytes and hashes them into a size_t
///
/// \param bytes - pointer to bytes to hash. Undefined behavior is bytes is NULL.
/// \param byte_count - number of bytes to hash. Must be greater than 0.
size_t cave_hash_bytes(uint8_t const * bytes, size_t byte_count);

/// \brief Takes a null-terminated string and hashes it.
///
/// \param bytes - the null terminated string,
size_t cave_hash_str(const char * bytes);

///@}

//   ><<     ><<      ><         ><< <<   ><<     ><<    ><<       ><<      ><       ><<<<<<<
//   ><<     ><<     >< <<     ><<    ><< ><<     ><<    >< ><<   ><<<     >< <<     ><<    ><<
//   ><<     ><<    ><  ><<     ><<       ><<     ><<    ><< ><< > ><<    ><  ><<    ><<    ><<
//   ><<<<<< ><<   ><<   ><<      ><<     ><<<<<< ><<    ><<  ><<  ><<   ><<   ><<   ><<<<<<<
//   ><<     ><<  ><<<<<< ><<        ><<  ><<     ><<    ><<   ><  ><<  ><<<<<< ><<  ><<
//   ><<     ><< ><<       ><< ><<    ><< ><<     ><<    ><<       ><< ><<       ><< ><<
//   ><<     ><<><<         ><<  ><< <<   ><<     ><<    ><<       ><<><<         ><<><<

/// \defgroup CaveHashMap CaveHashMap
///@{

/// A `cpy` variation of a function will call the associated `CAVE_<type>_CPY_FN` on the appropreate type.
/// A `mv` variation of a function will bitwise cpy the element into the destination, and free the memory the element
/// was being held in, but will not call a destructor.



/// A struct used by CaveHashMap to hold keys and values of runtime determined types.
typedef struct CaveKeyValue {
    void* key;
    void* value;
} CaveKeyValue;

/// \brief Helper function to compute a hash on arbitrary contiguous data.
size_t cave_hash_arbirary_bytes(void const * key, size_t byte_count);

///\defgroup CaveHashMap-Member-Function-Types CaveHashMap Member Function Types
///@{

///
typedef size_t (*CAVE_HASH_FN)(void const * key);

/// \brief Interface for users to define equality for CaveHashMap keys.
///
/// A `CAVE_KEY_EQ_FN` should return true if the two keys are "equal" and false if they are "not equal".
/// **IMPORTANT** In order for a function to be a valid `CAVE_KEY_EQ_FN`,
/// the function should not return true unless the hash of key_a == hash of key_b . Obviously this depends on
/// the hash function supplied. The bugs that are caused by this condition being violated are sporaddic and will
/// appear and disappear.
///
/// Useful for things like using case insensitive strings as keys.
typedef bool (*CAVE_KEY_EQ_FN)(void const * key_a, void const * key_b);

/// Must return `CAVE_NO_ERROR` on success. Can return an error for any reason, but generally
/// should stick to returning an error on allocation failure.
/// It should go without saying that copying a key should maintain equality (either bitwise or as
/// specified with a `CAVE_KEY_EQ_FN`) and should have the same hash under the specified
/// `CAVE_HASH_FN`.
typedef CaveError (*CAVE_KEY_CPY_FN)(CaveKeyValue* dest_key, CaveKeyValue const* src);

/// Must return `CAVE_NO_ERROR` on success. Can return an error for any reason, but generally
/// should stick to returning an error on allocation failure.
typedef CaveError (*CAVE_VALUE_CPY_FN)(CaveKeyValue* dest, CaveKeyValue const* src);

typedef void (*CAVE_KV_DESTRUCT_FN)(CaveKeyValue* kv);

//end of CaveHashMap-Member-Function-Types
/// @}


typedef struct CaveHashMap {
    size_t count;
    size_t capacity;
    size_t key_size;
    size_t value_size;
    CAVE_HASH_FN hash_fn;
    CAVE_KEY_EQ_FN key_eq_fn;
    CAVE_KEY_CPY_FN key_cpy_fn;
    CAVE_VALUE_CPY_FN value_cpy_fn;
    CAVE_KV_DESTRUCT_FN kv_destructor;
    /// A CaveVec<CaveVec<CaveKeyValue>>
    CaveVec buckets;
    /// A CaveVec<size_t>
    /// Contains list of indexes of buckets that are occupied.
    /// ie, all i such that buckets[i].len != 0.
    CaveVec occupied_buckets;
} CaveHashMap;


CaveHashMap* cave_hashmp_init(
        CaveHashMap* h,
        size_t value_size,
        size_t key_size,
        size_t min_capacity,
        CAVE_HASH_FN hash_fn,
        CaveError* err
        );

void cave_hashmp_set_key_eq_fn(CaveHashMap* h, CAVE_KEY_EQ_FN fn);
void cave_hashmp_set_key_cpy_fn(CaveHashMap* h, CAVE_KEY_CPY_FN fn);
void cave_hashmp_set_value_cpy_fn(CaveHashMap* h, CAVE_VALUE_CPY_FN fn);
void cave_hashmp_set_kv_destructor_fn(CaveHashMap* h, CAVE_KV_DESTRUCT_FN fn);


/// \brief Method to call on a CaveHashMap to free it's memory.
///
/// Calls the destructor on every CaveKeyValue held by `h` if there is one, then frees all memory `h` is holding.
///
/// \param h - Initialized and valid CaveHashMap. May be empty, but no prior release function may have been applied to `h`.
void cave_hashmp_release(CaveHashMap* h);

/// \brief Inserts `value` into the hashmap with the corresponding `key`. !!DOES NOT CHECK IF THE KEY EXISTS ALREADY!!
///
/// Creates a `CaveKeyValue`, which we will call `kv` which is inserted into `h` at the location
/// determined by `h->hash_fn(key)`. Then if `h->key_cpy_fn` is `NULL`, `h->key_size` bytes are copied from
/// `key` to `kv->key`. If it is not null, then `h->key_cpy_fn(kv->key, key)` is called.
/// Then if `h->value_cpy_fn` is `NULL`, `h->value_size` bytes are copied from
/// `value` to `kv->value`. If it is not null, then `h->value_cpy_fn(kv->key, key)` is called.
///
/// Increases the `h->count` by 1.
CaveHashMap* cave_hashmp_insert(CaveHashMap* h, void const * key, void const * value, CaveError* err);


/// \brief If `h` contains a key matching `key`, then updates it with `value`. If not, calls `cave_hashmp_insert(key, value, err)`.
CaveHashMap* cave_hashmp_update_or_insert(CaveHashMap* h, void const * key, void const * value, CaveError* err);


/// \brief Returns pointer to value associated with `key`
///
/// Finds a CaveKeyValue with a key that returns true when passed to `h->key_eq_fn` (which implies they have the same
/// hash value), and returns a pointer to the value field.
/// If no such CaveKeyValue can be found, returns null.
void* cave_hashmp_at(CaveHashMap* h, void const * key, CaveError* err);


/// \brief Removes the key-value pair that matches `key`.
///
/// Finds the key-value pair that matches `key`, which we will call `kv.
/// If `h->kv_destructor` is `NULL`, calls `free(kv->key)` and `free(kv->value).
/// Otherwise calls `h->kv_destructor(&kv)`.
CaveHashMap* cave_hashmap_remove(CaveHashMap* h, void const * key, CaveError* err);

/// \brief Copies the `CaveKeyValue` corrisponding to `key` into `dest`.
///.
/// If `h->key_cpy_fn` is not `NULL`, uses `h->key_cpy_fn` to copy the key into dest. Otherwise bitwise copies
/// `h->key_size` bytes into `dest->key`.
/// If `h->value_cpy_fn` is not `NULL`, uses `h->value_cpy_fn` to copy the value into dest. Otherwise bitwise copies
///// `h->value_size` bytes into `dest->value`.
CaveKeyValue* cave_hashmp_cpy_kv_into(CaveKeyValue* dest, CaveHashMap * h, void const * key, CaveError* err);

/// \brief Bitwise copies the `CaveKeyValue` corresponding to `key` into `dest`, then deletes it from `h`
/// without calling the destructor. Reduces `h.count` by 1.
CaveKeyValue* cave_hashmp_move_kv_into(CaveKeyValue* dest, CaveHashMap* h, void const * key, CaveError* err);

/// \brief Calls `cave_hashmp_remove` on every element in `h`.
CaveHashMap* cave_hashmp_clear(CaveHashMap* h);

/// \brief Returns a `CaveVec` with type `CaveKeyValue` that has a copy of every key-value pair in `h`.
///.
/// Uses the copy functions in `h` if they are not null to populate the `CaveKeyValue`'s in the returned `CaveVec`
/// if they are present, and bitwise copies the appropreate bytes if they are not.
CaveVec cave_hashmp_cpy_collect(CaveHashMap * h, CaveError* err);

/// \brief Returns a `CaveVec` with type `CaveKeyValue` that has had every element in `h` moved into it.
///
/// The move is done by bitwise copying every `CaveKeyValue` into the returned `CaveVec`, deleting it from `h`, but
/// not calling it's destructor.
CaveVec cave_hashmp_mv_collect(CaveHashMap* h, CaveError* err);


/// Rehashes `h` with at least `new_min_capacity` spaces.
///
/// If `new_min_capacity == 0`, then will be rehashed with some capacity greater than `h->capacity`.
/// This is done in such a way that if an error is returned via `err`, `h` is not invalidated
/// (still fine to use it) and the memory taken up trying to rehash is freed.
CaveHashMap* cave_hashmp_rehash(CaveHashMap* h, size_t new_min_capacity, CaveError* err);

size_t cave_hashmp_total_collisions(CaveHashMap * h);

/// \brief Finds the key with the most number of collisions, and returns how many collisions it has.
/// Optionally copies a key that hashes to the most collided hash into `key_dest`.

/// If `key_dest` is not `NULL` and `cave_hashmp_max_collisions` returns a non-zero value, copies the key into `key_dest`.
///
/// If `h->key_cpy_fn` is `NULL`, then the copying into `key_dest` bitwise copies `h->key_size` number of bytes. If
/// If `h->key_cpy_fn` is not `NULL`, then uses `h->key_cpy_fn` to copy the key into `key_dest`.
size_t cave_hashmp_max_collisions(CaveHashMap * h, void* key_dest, CaveError* err);

/// \brief Copies every CaveKeyValue in `src` into `dest`.
///
/// `dest` must be the same semantic type as `src`, which is to say they must be storing the same type of
/// key, value. Techncially they do not have to have the same equality, copy, and destructor functions, but
/// that's more because I can't think of a non-brittle way to enforce that than because it's semantically correct.
///
/// If `dest->key_cpy_fn` is not `NULL`, uses `dest->key_cpy_fn` to copy each key into `dest`. Otherwise bitwise copies
/// `dest->key_size` bytes into `dest->key`.
/// If `dest->value_cpy_fn` is not `NULL`, uses `dest->value_cpy_fn` to copy each value into `dest`. Otherwise bitwise copies
/// `dest->value_size` bytes into `dest->value`.
CaveHashMap* cave_hashmp_cpy_assign(CaveHashMap* dest, CaveHashMap * src, CaveError* err);

/// \brief Initializes `dest` to be compatable with `src`, then copies every CaveKeyValue in `src` into `dest` by
/// calling `cave_hashmp_cpy_assign(dest, src, err)`.
CaveHashMap* cave_hashmp_cpy_init(CaveHashMap* dest, CaveHashMap * src, CaveError* err);

//ToDo: in the future, add cave_hashmp_mv_assign and cave_hashmp_mv_init functions.


CaveHashMap* cave_hashmp_foreach(CaveHashMap * h, CAVE_FOREACH_CLOSURE fn, void* closure_data, CaveError* err);

///@}

#endif //CAVE_BEDROCK_H
