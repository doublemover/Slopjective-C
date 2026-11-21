#ifndef OBJC3_RUNTIME_STDLIB_GENERIC_COLLECTION_SERVICE_H
#define OBJC3_RUNTIME_STDLIB_GENERIC_COLLECTION_SERVICE_H

#include "objc3_runtime_services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_generic_array_opaque *objc3_generic_array_t;
typedef struct objc3_generic_view_opaque *objc3_generic_view_t;
typedef struct objc3_generic_iterator_opaque *objc3_generic_iterator_t;

typedef enum objc3_generic_collection_status {
  OBJC3_GENERIC_COLLECTION_OK = 0,
  OBJC3_GENERIC_COLLECTION_INVALID_ARGUMENT = 1,
  OBJC3_GENERIC_COLLECTION_INVALID_HANDLE = 2,
  OBJC3_GENERIC_COLLECTION_WRONG_KIND = 3,
  OBJC3_GENERIC_COLLECTION_STALE_HANDLE = 4,
  OBJC3_GENERIC_COLLECTION_OUT_OF_BOUNDS = 5,
  OBJC3_GENERIC_COLLECTION_CAPACITY_EXCEEDED = 6,
  OBJC3_GENERIC_COLLECTION_OUT_OF_MEMORY = 7,
  OBJC3_GENERIC_COLLECTION_MUTATED_DURING_ITERATION = 8,
  OBJC3_GENERIC_COLLECTION_UNSUPPORTED_OPERATION = 9,
  OBJC3_GENERIC_COLLECTION_CONCURRENT_ADVANCE = 10
} objc3_generic_collection_status;

/* Source and destination elements use descriptor stride align_up(size,
 * alignment). For a zero-sized payload, each callback still receives a valid,
 * aligned one-byte slot. create copies; create_move transfers each element
 * only as its move_init succeeds. Failed creation destroys only initialized
 * destination elements. All descriptor callbacks run without runtime locks. */
objc3_generic_collection_status objc3_generic_array_create(
    const objc3_value_ops *ops, const void *elements, uint64_t count,
    objc3_generic_array_t *out_array);
objc3_generic_collection_status objc3_generic_array_create_move(
    const objc3_value_ops *ops, void *elements, uint64_t count,
    objc3_generic_array_t *out_array);
objc3_generic_collection_status objc3_generic_array_retain(
    objc3_generic_array_t array, objc3_generic_array_t *out_array);
objc3_generic_collection_status objc3_generic_array_release(
    objc3_generic_array_t array);
objc3_generic_collection_status objc3_generic_array_count(
    objc3_generic_array_t array, uint64_t *out_count);
/* out_element must point to suitably aligned, uninitialized storage when the
 * payload has nonzero size. For a zero-sized payload it may be null; status
 * reports success and iterator has_value reports presence. Success initializes
 * caller storage, which the caller must later destroy with ops->destroy;
 * failures leave it uninitialized. */
objc3_generic_collection_status objc3_generic_array_get(
    objc3_generic_array_t array, uint64_t index, void *out_element);
/* Arrays keep immutable 32-element chunks behind a persistent index. Append
 * and set copy at most 32 payloads and one three-node metadata path; readers
 * retain their original snapshot until completion. Ordered remove still shifts
 * and copies the remaining elements. Publication checks the captured generation;
 * callbacks and retired-payload destruction run outside runtime locks. */
objc3_generic_collection_status objc3_generic_array_set(
    objc3_generic_array_t array, uint64_t index, const void *element);
objc3_generic_collection_status objc3_generic_array_append(
    objc3_generic_array_t array, const void *element);
objc3_generic_collection_status objc3_generic_array_remove(
    objc3_generic_array_t array, uint64_t index);
objc3_generic_collection_status objc3_generic_array_slice(
    objc3_generic_array_t array, uint64_t start, uint64_t count,
    objc3_generic_view_t *out_view);
objc3_generic_collection_status objc3_generic_view_release(
    objc3_generic_view_t view);
objc3_generic_collection_status objc3_generic_view_count(
    objc3_generic_view_t view, uint64_t *out_count);
objc3_generic_collection_status objc3_generic_view_get(
    objc3_generic_view_t view, uint64_t index, void *out_element);
objc3_generic_collection_status objc3_generic_array_iterator(
    objc3_generic_array_t array, objc3_generic_iterator_t *out_iterator);
objc3_generic_collection_status objc3_generic_iterator_release(
    objc3_generic_iterator_t iterator);
/* has_value explicitly distinguishes exhaustion from a present zero/null
 * payload. out_element is initialized only when has_value becomes one. */
objc3_generic_collection_status objc3_generic_iterator_next(
    objc3_generic_iterator_t iterator, int *out_has_value, void *out_element);

#ifdef __cplusplus
}
#endif
#endif
