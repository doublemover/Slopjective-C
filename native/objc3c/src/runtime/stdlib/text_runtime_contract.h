#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum objc3_runtime_stdlib_text_status_i32 {
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OK = 0,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_HANDLE = 30641,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_INVALID_SHAPE = 30642,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_UTF8 = 30643,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUT_OF_BOUNDS = 30644,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OUTPUT_TOO_SMALL = 30645,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STORAGE_UNAVAILABLE = 30646,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CROSS_KIND_HANDLE = 30647,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_STALE_HANDLE = 30648,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MALFORMED_DESCRIPTOR = 30649,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_CAPACITY_EXCEEDED = 30650,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_MUTATED_DURING_ITERATION = 30651,
  OBJC3_RUNTIME_STDLIB_TEXT_STATUS_OVERFLOW = 30652,
} objc3_runtime_stdlib_text_status_i32;

typedef struct objc3_runtime_stdlib_text_snapshot {
  uint64_t reset_generation;
  uint64_t total_call_count;
  uint64_t literal_call_count;
  uint64_t query_call_count;
  uint64_t concat_call_count;
  uint64_t storage_create_call_count;
  uint64_t storage_query_call_count;
  uint64_t status_call_count;
  int text_record_count;
  int owned_storage_record_count;
  int owned_storage_byte_count;
  int last_handle;
  int last_input_a;
  int last_input_b;
  int last_input_c;
  int last_status;
  int last_result;
  uint32_t abi_version;
  uint32_t handle_generation;
  uint64_t mutation_generation;
  uint64_t invalid_handle_failure_count;
  uint64_t cross_kind_handle_failure_count;
  uint64_t stale_handle_failure_count;
  uint64_t malformed_descriptor_failure_count;
  uint64_t capacity_failure_count;
  uint64_t iterator_invalidation_count;
  int literal_record_count;
  int builder_record_count;
  int scalar_iterator_record_count;
  int malformed_record_count;
  int stale_record_count;
  int reserved_normalized_record_count;
  int reserved_collated_record_count;
} objc3_runtime_stdlib_text_snapshot;

int objc3_runtime_stdlib_text_utf8_literal_i32(int byte_count,
                                               int unit_count,
                                               int valid_utf8);
int objc3_runtime_stdlib_text_utf8_storage_i32(const char *utf8_bytes,
                                               int byte_count);
int objc3_runtime_stdlib_text_byte_count_i32(int handle);
int objc3_runtime_stdlib_text_unit_count_i32(int handle);
int objc3_runtime_stdlib_text_scalar_count_i32(int handle);
int objc3_runtime_stdlib_text_is_valid_utf8_i32(int handle);
int objc3_runtime_stdlib_text_byte_at_or_i32(int handle,
                                             int byte_index,
                                             int default_value);
int objc3_runtime_stdlib_text_prefix_units_i32(int handle, int requested_units);
int objc3_runtime_stdlib_text_concat_i32(int left_handle, int right_handle);
int objc3_runtime_stdlib_text_append_utf8_storage_i32(int handle,
                                                      const char *utf8_bytes,
                                                      int byte_count);
int objc3_runtime_stdlib_text_builder_i32(void);
int objc3_runtime_stdlib_text_builder_append_utf8_i32(int builder_handle,
                                                      const char *utf8_bytes,
                                                      int byte_count);
int objc3_runtime_stdlib_text_builder_build_i32(int builder_handle);
int objc3_runtime_stdlib_text_scalar_iterator_i32(int handle);
int objc3_runtime_stdlib_text_scalar_iterator_next_or_i32(int iterator_handle,
                                                          int default_value);
int objc3_runtime_stdlib_text_equal_i32(int left_handle, int right_handle);
int objc3_runtime_stdlib_text_last_status_i32(void);
int objc3_runtime_copy_stdlib_text_utf8_bytes_for_testing(int handle,
                                                          char *out,
                                                          int capacity);
int objc3_runtime_copy_stdlib_text_state_for_testing(
    objc3_runtime_stdlib_text_snapshot *out);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace objc3c::runtime {

void ResetRuntimeStdlibTextStateForTesting();

}  // namespace objc3c::runtime
#endif
