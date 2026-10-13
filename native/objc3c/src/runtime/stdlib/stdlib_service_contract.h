#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Version 1 int handles remain a separate compatibility domain. These
// functions retain/release only handles returned by the matching legacy text
// or collections API above.
int objc3_runtime_stdlib_text_retain_i32(int handle);
int objc3_runtime_stdlib_text_release_i32(int handle);
int objc3_runtime_stdlib_collections_retain_i32(int handle);
int objc3_runtime_stdlib_collections_release_i32(int handle);

// Typed opaque v2 domains. Values are opaque identities and are never native
// Objective-C object IDs or dereferenceable user pointers.
typedef struct objc3_text_v2_opaque *objc3_text_v2_t;
typedef struct objc3_collection_v2_opaque *objc3_collection_v2_t;
typedef objc3_collection_v2_t objc3_array_i32_v2_t;
typedef struct objc3_slice_i32_v2_opaque *objc3_slice_i32_v2_t;
typedef struct objc3_iterator_i32_v2_opaque *objc3_iterator_i32_v2_t;
typedef struct objc3_map_i32_i32_v2_opaque *objc3_map_i32_i32_v2_t;
typedef struct objc3_set_i32_v2_opaque *objc3_set_i32_v2_t;
typedef struct objc3_text_builder_v2_opaque *objc3_text_builder_v2_t;

typedef enum objc3_runtime_stdlib_v2_status {
  OBJC3_STDLIB_V2_OK = 0,
  OBJC3_STDLIB_V2_INVALID_ARGUMENT = 1,
  OBJC3_STDLIB_V2_INVALID_HANDLE = 2,
  OBJC3_STDLIB_V2_WRONG_KIND = 3,
  OBJC3_STDLIB_V2_STALE_HANDLE = 4,
  OBJC3_STDLIB_V2_INVALID_UTF8 = 5,
  OBJC3_STDLIB_V2_OUT_OF_BOUNDS = 6,
  OBJC3_STDLIB_V2_OVERFLOW = 7,
  OBJC3_STDLIB_V2_CAPACITY_EXCEEDED = 8,
  OBJC3_STDLIB_V2_OUT_OF_MEMORY = 9,
  OBJC3_STDLIB_V2_MUTATED_DURING_ITERATION = 10,
  OBJC3_STDLIB_V2_NOT_FOUND = 11
} objc3_runtime_stdlib_v2_status;

// Checked UTF-8 operations use byte lengths and byte offsets explicitly.
// Text inputs must be valid UTF-8; embedded NUL is ordinary data.
int objc3_runtime_stdlib_text_v2_create_utf8_checked(
    const char *bytes, uint64_t byte_count, objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_v2_retain_checked(objc3_text_v2_t text,
                                                 objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_v2_release_checked(objc3_text_v2_t text);
int objc3_runtime_stdlib_text_v2_byte_count_checked(objc3_text_v2_t text,
                                                     uint64_t *out_count);
int objc3_runtime_stdlib_text_v2_byte_at_checked(objc3_text_v2_t text,
                                                  uint64_t index,
                                                  uint8_t *out_byte);
int objc3_runtime_stdlib_text_v2_scalar_count_checked(objc3_text_v2_t text,
                                                       uint64_t *out_count);
int objc3_runtime_stdlib_text_v2_slice_scalars(
    objc3_text_v2_t text, uint64_t start_scalar, uint64_t scalar_count,
    objc3_text_v2_t *out_text);
// Scalar-index search only reports matches whose beginning and end are UTF-8
// scalar boundaries. start_scalar and the returned index count Unicode scalar
// values, not bytes or grapheme clusters. The needle must be valid UTF-8.
// Empty needles match start_scalar, including the end of the text. A start
// beyond the scalar count returns OUT_OF_BOUNDS; no match returns OK/found=0.
int objc3_runtime_stdlib_text_v2_find_scalars(
    objc3_text_v2_t text, const char *needle_utf8, uint64_t needle_byte_count,
    uint64_t start_scalar, int *out_found, uint64_t *out_scalar_index);
int objc3_runtime_stdlib_text_v2_find_bytes(
    objc3_text_v2_t text, const char *needle, uint64_t needle_byte_count,
    uint64_t start_byte, int *out_found, uint64_t *out_byte_index);
int objc3_runtime_stdlib_text_v2_concat(
    objc3_text_v2_t left, objc3_text_v2_t right,
    objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_builder_v2_create(
    objc3_text_builder_v2_t *out_builder);
int objc3_runtime_stdlib_text_builder_v2_append_utf8(
    objc3_text_builder_v2_t builder, const char *bytes, uint64_t byte_count);
// Appends one Unicode scalar, encoded as UTF-8. Surrogates and out-of-range
// code points return INVALID_ARGUMENT without changing the builder.
int objc3_runtime_stdlib_text_builder_v2_append_scalar(
    objc3_text_builder_v2_t builder, uint32_t scalar);
int objc3_runtime_stdlib_text_builder_v2_append_text(
    objc3_text_builder_v2_t builder, objc3_text_v2_t text);
// Finish produces an independent text snapshot and preserves builder contents.
// Reset clears those contents. Both operations leave the builder reusable.
int objc3_runtime_stdlib_text_builder_v2_finish(
    objc3_text_builder_v2_t builder, objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_builder_v2_reset(
    objc3_text_builder_v2_t builder);
int objc3_runtime_stdlib_text_builder_v2_release(
    objc3_text_builder_v2_t builder);

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

objc3_text_v2_t objc3_runtime_stdlib_text_v2_create_utf8(
    const char *utf8_bytes, uint64_t byte_count);
objc3_text_v2_t objc3_runtime_stdlib_text_v2_retain(objc3_text_v2_t text);
void objc3_runtime_stdlib_text_v2_release(objc3_text_v2_t text);
int objc3_runtime_stdlib_text_v2_byte_count(objc3_text_v2_t text,
                                            uint64_t *out_count);
int objc3_runtime_stdlib_text_v2_byte_at(objc3_text_v2_t text,
                                         uint64_t index,
                                         uint8_t *out_byte);

objc3_collection_v2_t objc3_runtime_stdlib_collections_v2_create_i32(
    const int32_t *values, uint64_t count);
objc3_collection_v2_t objc3_runtime_stdlib_collections_v2_retain(
    objc3_collection_v2_t collection);
void objc3_runtime_stdlib_collections_v2_release(
    objc3_collection_v2_t collection);
int objc3_runtime_stdlib_collections_v2_count(objc3_collection_v2_t collection,
                                              uint64_t *out_count);
int objc3_runtime_stdlib_collections_v2_get(objc3_collection_v2_t collection,
                                             uint64_t index,
                                             int32_t *out_value);

#ifdef __cplusplus
}
#endif
