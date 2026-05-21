#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum objc3_runtime_stdlib_collections_status_i32 {
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OK = 0,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_HANDLE = 30631,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OUT_OF_BOUNDS = 30632,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_COUNT = 30633,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_NOT_FOUND = 30634,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_INVALID_RANGE = 30635,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_ITERATION_END = 30636,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MUTATED_DURING_ITERATION = 30637,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CROSS_KIND_HANDLE = 30638,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_STALE_HANDLE = 30639,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_MALFORMED_DESCRIPTOR = 30640,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_DESCRIPTOR_MISMATCH = 30641,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_CAPACITY_EXCEEDED = 30653,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_STATUS_OVERFLOW = 30654,
} objc3_runtime_stdlib_collections_status_i32;

typedef enum objc3_runtime_stdlib_collections_descriptor_kind_i32 {
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ARRAY = 1,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MUTABLE_ARRAY = 2,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SLICE = 3,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_MAP = 4,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_SET = 5,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_ITERATOR = 6,
} objc3_runtime_stdlib_collections_descriptor_kind_i32;

typedef enum objc3_runtime_stdlib_collections_descriptor_type_i32 {
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_NONE = 0,
  OBJC3_RUNTIME_STDLIB_COLLECTIONS_DESCRIPTOR_TYPE_I32 = 1,
} objc3_runtime_stdlib_collections_descriptor_type_i32;

typedef struct objc3_runtime_stdlib_collections_snapshot {
  uint64_t reset_generation;
  uint64_t total_call_count;
  uint64_t array_create_call_count;
  uint64_t array_query_call_count;
  uint64_t map_create_call_count;
  uint64_t map_query_call_count;
  uint64_t set_create_call_count;
  uint64_t set_query_call_count;
  uint64_t set_mutation_call_count;
  uint64_t slice_create_call_count;
  uint64_t slice_query_call_count;
  uint64_t iterator_create_call_count;
  uint64_t iterator_query_call_count;
  uint64_t status_call_count;
  int array_record_count;
  int map_record_count;
  int set_record_count;
  int slice_record_count;
  int iterator_record_count;
  int last_handle;
  int last_input_a;
  int last_input_b;
  int last_input_c;
  int last_status;
  int last_result;
  uint64_t map_mutation_call_count;
  uint32_t abi_version;
  uint32_t handle_generation;
  uint64_t mutation_generation;
  uint64_t invalid_handle_failure_count;
  uint64_t cross_kind_handle_failure_count;
  uint64_t stale_handle_failure_count;
  uint64_t malformed_descriptor_failure_count;
  uint64_t capacity_failure_count;
  uint64_t iterator_invalidation_count;
  int immutable_array_record_count;
  int mutable_array_record_count;
  int descriptor_record_count;
  int stale_record_count;
} objc3_runtime_stdlib_collections_snapshot;

int objc3_runtime_stdlib_collections_array3_i32(int first,
                                                int second,
                                                int third,
                                                int count);
int objc3_runtime_stdlib_collections_descriptor_i32(int descriptor_kind,
                                                   int key_type,
                                                   int value_type);
int objc3_runtime_stdlib_collections_descriptor_matches_i32(
    int descriptor_handle,
    int collection_handle);
int objc3_runtime_stdlib_collections_array3_descriptor_i32(
    int descriptor_handle,
    int first,
    int second,
    int third,
    int count);
int objc3_runtime_stdlib_collections_array_storage_i32(const int *values,
                                                       int count);
int objc3_runtime_stdlib_collections_mutable_array_i32(void);
int objc3_runtime_stdlib_collections_mutable_array_append_i32(int handle,
                                                              int value);
int objc3_runtime_stdlib_collections_mutable_array_set_i32(int handle,
                                                           int index,
                                                           int value);
int objc3_runtime_stdlib_collections_mutable_array_remove_at_i32(int handle,
                                                                 int index);
int objc3_runtime_stdlib_collections_array_count_i32(int handle);
int objc3_runtime_stdlib_collections_array_get_or_i32(int handle,
                                                      int index,
                                                      int default_value);
int objc3_runtime_stdlib_collections_array_prefix_count_i32(int handle,
                                                            int requested);
int objc3_runtime_stdlib_collections_array_sum_i32(int handle);
int objc3_runtime_stdlib_collections_array_slice_i32(int handle,
                                                     int start,
                                                     int count);
int objc3_runtime_stdlib_collections_array_iterator_i32(int handle);
int objc3_runtime_stdlib_collections_map_entry_i32(int key, int value);
int objc3_runtime_stdlib_collections_map_entry_descriptor_i32(
    int descriptor_handle,
    int key,
    int value);
int objc3_runtime_stdlib_collections_map_empty_i32(void);
int objc3_runtime_stdlib_collections_map_count_i32(int handle);
int objc3_runtime_stdlib_collections_map_contains_i32(int handle, int key);
int objc3_runtime_stdlib_collections_map_insert_i32(int handle,
                                                    int key,
                                                    int value);
int objc3_runtime_stdlib_collections_map_delete_i32(int handle, int key);
int objc3_runtime_stdlib_collections_map_lookup_or_i32(int handle,
                                                       int key,
                                                       int default_value);
int objc3_runtime_stdlib_collections_map_key_iterator_i32(int handle);
int objc3_runtime_stdlib_collections_map_value_iterator_i32(int handle);
int objc3_runtime_stdlib_collections_set3_i32(int first,
                                             int second,
                                             int third,
                                             int count);
int objc3_runtime_stdlib_collections_set3_descriptor_i32(int descriptor_handle,
                                                        int first,
                                                        int second,
                                                        int third,
                                                        int count);
int objc3_runtime_stdlib_collections_set_storage_i32(const int *values,
                                                     int count);
int objc3_runtime_stdlib_collections_set_count_i32(int handle);
int objc3_runtime_stdlib_collections_set_contains_i32(int handle, int value);
int objc3_runtime_stdlib_collections_set_insert_i32(int handle, int value);
int objc3_runtime_stdlib_collections_set_delete_i32(int handle, int value);
int objc3_runtime_stdlib_collections_set_iterator_i32(int handle);
int objc3_runtime_stdlib_collections_slice_count_i32(int handle);
int objc3_runtime_stdlib_collections_slice_get_or_i32(int handle,
                                                     int index,
                                                     int default_value);
int objc3_runtime_stdlib_collections_slice_iterator_i32(int handle);
int objc3_runtime_stdlib_collections_iterator_next_or_i32(int handle,
                                                         int default_value);
int objc3_runtime_stdlib_collections_last_status_i32(void);
int objc3_runtime_copy_stdlib_collections_state_for_testing(
    objc3_runtime_stdlib_collections_snapshot *out);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace objc3c::runtime {

void ResetRuntimeStdlibCollectionsStateForTesting();

}  // namespace objc3c::runtime
#endif
