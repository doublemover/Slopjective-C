#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_arc_debug_state_snapshot {
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
  int last_retain_value;
  int last_release_value;
  int last_autorelease_value;
  int last_property_read_value;
  int last_property_written_value;
  int last_property_exchange_previous_value;
  int last_property_exchange_new_value;
  int last_weak_loaded_value;
  int last_weak_stored_value;
  int last_property_receiver;
  const char *last_property_name;
  const char *last_property_owner_identity;
} objc3_runtime_arc_debug_state_snapshot;

int objc3_runtime_copy_arc_debug_state_for_testing(
    objc3_runtime_arc_debug_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
