#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_block_arc_runtime_abi_snapshot {
  uint64_t private_runtime_abi_ready;
  uint64_t public_runtime_header_unchanged;
  uint64_t deterministic;
  uint64_t live_runtime_block_handle_count;
  uint64_t block_promote_call_count;
  uint64_t block_invoke_call_count;
  uint64_t retain_call_count;
  uint64_t release_call_count;
  uint64_t autorelease_call_count;
  uint64_t autoreleasepool_push_count;
  uint64_t autoreleasepool_pop_count;
  uint64_t current_property_read_count;
  uint64_t current_property_write_count;
  uint64_t current_property_exchange_count;
  uint64_t weak_current_property_load_count;
  uint64_t weak_current_property_store_count;
  uint64_t last_descriptor_address;
  uint64_t last_descriptor_invoke_address;
  uint64_t last_descriptor_storage_size_bytes;
  uint64_t last_descriptor_capture_count;
  uint64_t last_descriptor_storage_word_count;
  uint64_t last_descriptor_flags;
  uint64_t last_invoke_plan_storage_word_count;
  int last_promoted_block_handle;
  int last_promote_has_pointer_capture_storage;
  int last_descriptor_parameter_count;
  int last_descriptor_has_invoke;
  int last_descriptor_has_copy_helper;
  int last_descriptor_has_dispose_helper;
  int last_descriptor_has_pointer_capture_storage;
  int last_invoke_plan_has_descriptor;
  int last_invoke_plan_has_invoke;
  int last_invoke_plan_was_runnable;
  int last_invoked_block_handle;
  int last_block_invoke_result;
  int last_retain_value;
  int last_release_value;
  int last_autorelease_value;
  const char *block_promote_symbol;
  const char *block_invoke_symbol;
  const char *retain_symbol;
  const char *release_symbol;
  const char *autorelease_symbol;
  const char *autoreleasepool_push_symbol;
  const char *autoreleasepool_pop_symbol;
  const char *current_property_read_symbol;
  const char *current_property_write_symbol;
  const char *current_property_exchange_symbol;
  const char *bind_current_property_context_symbol;
  const char *clear_current_property_context_symbol;
  const char *weak_current_property_load_symbol;
  const char *weak_current_property_store_symbol;
  const char *arc_debug_state_snapshot_symbol;
  const char *runtime_abi_boundary_model;
  const char *block_runtime_model;
  const char *block_descriptor_model;
  const char *block_invoke_thunk_model;
  const char *arc_runtime_model;
  const char *fail_closed_model;
} objc3_runtime_block_arc_runtime_abi_snapshot;

int objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
