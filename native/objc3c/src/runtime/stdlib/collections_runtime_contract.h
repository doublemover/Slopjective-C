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
} objc3_runtime_stdlib_collections_status_i32;

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
} objc3_runtime_stdlib_collections_snapshot;

int objc3_runtime_stdlib_collections_array3_i32(int first,
                                                int second,
                                                int third,
                                                int count);
int objc3_runtime_stdlib_collections_array_count_i32(int handle);
int objc3_runtime_stdlib_collections_array_get_or_i32(int handle,
                                                      int index,
                                                      int default_value);
int objc3_runtime_stdlib_collections_array_prefix_count_i32(int handle,
                                                            int requested);
int objc3_runtime_stdlib_collections_array_slice_i32(int handle,
                                                     int start,
                                                     int count);
int objc3_runtime_stdlib_collections_array_iterator_i32(int handle);
int objc3_runtime_stdlib_collections_map_entry_i32(int key, int value);
int objc3_runtime_stdlib_collections_map_count_i32(int handle);
int objc3_runtime_stdlib_collections_map_contains_i32(int handle, int key);
int objc3_runtime_stdlib_collections_map_lookup_or_i32(int handle,
                                                       int key,
                                                       int default_value);
int objc3_runtime_stdlib_collections_set3_i32(int first,
                                             int second,
                                             int third,
                                             int count);
int objc3_runtime_stdlib_collections_set_count_i32(int handle);
int objc3_runtime_stdlib_collections_set_contains_i32(int handle, int value);
int objc3_runtime_stdlib_collections_set_insert_i32(int handle, int value);
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
