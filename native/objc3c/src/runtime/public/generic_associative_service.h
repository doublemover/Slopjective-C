#ifndef OBJC3_RUNTIME_STDLIB_GENERIC_ASSOCIATIVE_SERVICE_H
#define OBJC3_RUNTIME_STDLIB_GENERIC_ASSOCIATIVE_SERVICE_H

#include "generic_collection_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_generic_map_opaque *objc3_generic_map_t;
typedef struct objc3_generic_set_opaque *objc3_generic_set_t;
typedef struct objc3_generic_associative_iterator_opaque
    *objc3_generic_associative_iterator_t;

typedef objc3_service_status (*objc3_generic_hash_fn)(const void *key,
                                                       uint64_t *out_hash);
typedef objc3_service_status (*objc3_generic_equal_fn)(const void *left,
    const void *right, int *out_equal);

/* The descriptor and callback code must outlive the map/set and its iterators.
 * Hash must be stable for an initialized key. Equal keys must have equal hashes.
 * Callbacks do not throw across this C ABI. */
typedef struct objc3_generic_map_descriptor {
  uint32_t struct_size;
  uint32_t version;
  objc3_value_ops key_ops;
  objc3_value_ops value_ops;
  objc3_generic_hash_fn hash;
  objc3_generic_equal_fn equal;
} objc3_generic_map_descriptor;

typedef struct objc3_generic_set_descriptor {
  uint32_t struct_size;
  uint32_t version;
  objc3_value_ops key_ops;
  objc3_generic_hash_fn hash;
  objc3_generic_equal_fn equal;
} objc3_generic_set_descriptor;

#define OBJC3_GENERIC_ASSOCIATIVE_DESCRIPTOR_VERSION 1u

objc3_generic_collection_status objc3_generic_map_create_empty(
    const objc3_generic_map_descriptor *descriptor,
    objc3_generic_map_t *out_map);
/* Keys and values are separate contiguous spans, each using its own descriptor
 * stride. Creation applies insert semantics in source order. */
objc3_generic_collection_status objc3_generic_map_create(
    const objc3_generic_map_descriptor *descriptor, const void *keys,
    const void *values, uint64_t count, objc3_generic_map_t *out_map);
objc3_generic_collection_status objc3_generic_map_retain(
    objc3_generic_map_t map, objc3_generic_map_t *out_map);
objc3_generic_collection_status objc3_generic_map_release(
    objc3_generic_map_t map);
objc3_generic_collection_status objc3_generic_map_count(
    objc3_generic_map_t map, uint64_t *out_count);
/* inserted is one for a new key, zero for replacement. Duplicate replacement
 * keeps the original key slot and insertion order. */
objc3_generic_collection_status objc3_generic_map_insert(
    objc3_generic_map_t map, const void *key, const void *value,
    int *out_inserted);
objc3_generic_collection_status objc3_generic_map_lookup(
    objc3_generic_map_t map, const void *key, int *out_has_value,
    void *out_value);
/* out_has_value reports membership even if copying the found payload fails;
 * status describes that copy failure and out_value remains uninitialized. */
objc3_generic_collection_status objc3_generic_map_remove(
    objc3_generic_map_t map, const void *key, int *out_removed);
objc3_generic_collection_status objc3_generic_map_iterator(
    objc3_generic_map_t map, objc3_generic_associative_iterator_t *out_iterator);

objc3_generic_collection_status objc3_generic_set_create_empty(
    const objc3_generic_set_descriptor *descriptor,
    objc3_generic_set_t *out_set);
objc3_generic_collection_status objc3_generic_set_create(
    const objc3_generic_set_descriptor *descriptor, const void *values,
    uint64_t count, objc3_generic_set_t *out_set);
objc3_generic_collection_status objc3_generic_set_retain(
    objc3_generic_set_t set, objc3_generic_set_t *out_set);
objc3_generic_collection_status objc3_generic_set_release(
    objc3_generic_set_t set);
objc3_generic_collection_status objc3_generic_set_count(
    objc3_generic_set_t set, uint64_t *out_count);
objc3_generic_collection_status objc3_generic_set_insert(
    objc3_generic_set_t set, const void *value, int *out_inserted);
objc3_generic_collection_status objc3_generic_set_contains(
    objc3_generic_set_t set, const void *value, int *out_contains);
objc3_generic_collection_status objc3_generic_set_remove(
    objc3_generic_set_t set, const void *value, int *out_removed);
objc3_generic_collection_status objc3_generic_set_iterator(
    objc3_generic_set_t set, objc3_generic_associative_iterator_t *out_iterator);

objc3_generic_collection_status objc3_generic_associative_iterator_release(
    objc3_generic_associative_iterator_t iterator);
/* Map iterators initialize both outputs; set iterators initialize only key.
 * For zero-sized payloads the corresponding output may be null. */
objc3_generic_collection_status objc3_generic_associative_iterator_next(
    objc3_generic_associative_iterator_t iterator, int *out_has_value,
    void *out_key, void *out_value);

#ifdef __cplusplus
}
#endif
#endif
