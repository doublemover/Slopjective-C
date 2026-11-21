#ifndef OBJC3_RUNTIME_PUBLIC_OBJC3_COLLECTIONS_H
#define OBJC3_RUNTIME_PUBLIC_OBJC3_COLLECTIONS_H

#include "objc3_stdlib_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_collection_v2_opaque *objc3_collection_v2_t;
typedef objc3_collection_v2_t objc3_array_i32_v2_t;
typedef struct objc3_slice_i32_v2_opaque *objc3_slice_i32_v2_t;
typedef struct objc3_iterator_i32_v2_opaque *objc3_iterator_i32_v2_t;
typedef struct objc3_map_i32_i32_v2_opaque *objc3_map_i32_i32_v2_t;
typedef struct objc3_set_i32_v2_opaque *objc3_set_i32_v2_t;

// Concrete i32 collections. Map iteration is insertion ordered; replacing a
// key preserves its position. All bounds and mutation failures are explicit.
int objc3_runtime_stdlib_array_i32_v2_create(
    const int32_t *values, uint64_t count, objc3_array_i32_v2_t *out_array);
int objc3_runtime_stdlib_array_i32_v2_retain(
    objc3_array_i32_v2_t array, objc3_array_i32_v2_t *out_array);
int objc3_runtime_stdlib_array_i32_v2_release(objc3_array_i32_v2_t array);
int objc3_runtime_stdlib_array_i32_v2_count(objc3_array_i32_v2_t array,
                                            uint64_t *out_count);
int objc3_runtime_stdlib_array_i32_v2_get(objc3_array_i32_v2_t array,
                                          uint64_t index,
                                          int32_t *out_value);
int objc3_runtime_stdlib_array_i32_v2_set(objc3_array_i32_v2_t array,
                                          uint64_t index, int32_t value);
int objc3_runtime_stdlib_array_i32_v2_append(objc3_array_i32_v2_t array,
                                             int32_t value);
int objc3_runtime_stdlib_array_i32_v2_remove(objc3_array_i32_v2_t array,
                                             uint64_t index,
                                             int32_t *out_value);
int objc3_runtime_stdlib_array_i32_v2_slice(
    objc3_array_i32_v2_t array, uint64_t start, uint64_t count,
    objc3_slice_i32_v2_t *out_slice);
int objc3_runtime_stdlib_slice_i32_v2_release(objc3_slice_i32_v2_t slice);
int objc3_runtime_stdlib_slice_i32_v2_count(objc3_slice_i32_v2_t slice,
                                            uint64_t *out_count);
int objc3_runtime_stdlib_slice_i32_v2_get(objc3_slice_i32_v2_t slice,
                                          uint64_t index,
                                          int32_t *out_value);
int objc3_runtime_stdlib_array_i32_v2_iterator(
    objc3_array_i32_v2_t array, objc3_iterator_i32_v2_t *out_iterator);
int objc3_runtime_stdlib_iterator_i32_v2_release(
    objc3_iterator_i32_v2_t iterator);
int objc3_runtime_stdlib_iterator_i32_v2_next(
    objc3_iterator_i32_v2_t iterator, int *out_has_value,
    int32_t *out_value);

/* Concrete i32 map/set membership and append use a private integer hash index
 * with expected average constant-time lookup. Ordered removal repairs shifted
 * slots in O(N). Duplicate set insertion and unchanged map values preserve
 * iterator validity; allocation failure publishes neither payload nor index. */
int objc3_runtime_stdlib_map_i32_i32_v2_create(
    objc3_map_i32_i32_v2_t *out_map);
int objc3_runtime_stdlib_map_i32_i32_v2_release(
    objc3_map_i32_i32_v2_t map);
int objc3_runtime_stdlib_map_i32_i32_v2_count(objc3_map_i32_i32_v2_t map,
                                              uint64_t *out_count);
int objc3_runtime_stdlib_map_i32_i32_v2_contains(
    objc3_map_i32_i32_v2_t map, int32_t key, int *out_found);
int objc3_runtime_stdlib_map_i32_i32_v2_get(
    objc3_map_i32_i32_v2_t map, int32_t key, int *out_found,
    int32_t *out_value);
int objc3_runtime_stdlib_map_i32_i32_v2_insert(
    objc3_map_i32_i32_v2_t map, int32_t key, int32_t value);
/* Returns the count from the same locked mutation, including replacement.
 * Failure clears out_count. */
int objc3_runtime_stdlib_map_i32_i32_v2_insert_count(
    objc3_map_i32_i32_v2_t map, int32_t key, int32_t value, uint64_t *out_count);
int objc3_runtime_stdlib_map_i32_i32_v2_remove(
    objc3_map_i32_i32_v2_t map, int32_t key, int *out_removed);
int objc3_runtime_stdlib_map_i32_i32_v2_iterator(
    objc3_map_i32_i32_v2_t map, objc3_iterator_i32_v2_t *out_iterator);
int objc3_runtime_stdlib_map_i32_i32_v2_iterator_next(
    objc3_iterator_i32_v2_t iterator, int *out_has_value, int32_t *out_key,
    int32_t *out_value);

int objc3_runtime_stdlib_set_i32_v2_create(objc3_set_i32_v2_t *out_set);
int objc3_runtime_stdlib_set_i32_v2_release(objc3_set_i32_v2_t set);
int objc3_runtime_stdlib_set_i32_v2_count(objc3_set_i32_v2_t set,
                                          uint64_t *out_count);
int objc3_runtime_stdlib_set_i32_v2_contains(objc3_set_i32_v2_t set,
                                             int32_t value, int *out_found);
int objc3_runtime_stdlib_set_i32_v2_insert(objc3_set_i32_v2_t set,
                                           int32_t value, int *out_inserted);
int objc3_runtime_stdlib_set_i32_v2_remove(objc3_set_i32_v2_t set,
                                           int32_t value, int *out_removed);
int objc3_runtime_stdlib_set_i32_v2_iterator(
    objc3_set_i32_v2_t set, objc3_iterator_i32_v2_t *out_iterator);


#ifdef __cplusplus
}
#endif
#endif
