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
