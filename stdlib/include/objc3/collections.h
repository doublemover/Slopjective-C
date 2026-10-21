#ifndef OBJC3_COLLECTIONS_H
#define OBJC3_COLLECTIONS_H

#include "core.h"
#include <objc3c/runtime/objc3_collections.h>

#define OBJC3_COLLECTIONS_CAPABILITY_ID "objc3.cap.collections"
#include <objc3c/runtime/generic_collection_service.h>
#include <objc3c/runtime/generic_associative_service.h>

/* Failure remains observable while the caller's default is written. */
static inline int objc3_array_i32_get_or(objc3_array_i32_v2_t array,
    uint64_t index, int32_t fallback, int32_t *out_value) {
  if (!out_value) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_value = fallback;
  int32_t value = 0;
  int status = objc3_runtime_stdlib_array_i32_v2_get(array, index, &value);
  if (status == OBJC3_STDLIB_V2_OK) *out_value = value;
  return status;
}

static inline int objc3_array_i32_prefix_count(objc3_array_i32_v2_t array,
    uint64_t requested, uint64_t *out_count) {
  if (!out_count) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_count = 0;
  uint64_t count = 0;
  int status = objc3_runtime_stdlib_array_i32_v2_count(array, &count);
  if (status == OBJC3_STDLIB_V2_OK)
    *out_count = requested < count ? requested : count;
  return status;
}

static inline int objc3_map_i32_get_or(objc3_map_i32_i32_v2_t map,
    int32_t key, int32_t fallback, int32_t *out_value) {
  if (!out_value) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_value = fallback;
  int found = 0;
  int32_t value = 0;
  int status = objc3_runtime_stdlib_map_i32_i32_v2_get(map, key, &found, &value);
  if (status != OBJC3_STDLIB_V2_OK) return status;
  if (!found) return OBJC3_STDLIB_V2_NOT_FOUND;
  *out_value = value;
  return OBJC3_STDLIB_V2_OK;
}

/* Insert preserves replacement semantics and reports the current entry count.
 * All failures clear out_count; mutation failure never masquerades as a count. */
static inline int objc3_map_i32_insert_count(objc3_map_i32_i32_v2_t map,
    int32_t key, int32_t value, uint64_t *out_count) {
  return objc3_runtime_stdlib_map_i32_i32_v2_insert_count(map, key, value, out_count);
}

#endif
