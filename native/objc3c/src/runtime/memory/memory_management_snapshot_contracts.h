#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_memory_management_state_snapshot {
  uint64_t live_runtime_instance_count;
  uint64_t weak_target_count;
  uint64_t weak_slot_ref_count;
  uint64_t autoreleasepool_depth;
  uint64_t autoreleasepool_max_depth;
  uint64_t queued_autorelease_value_count;
  uint64_t drained_autorelease_value_count;
  int last_autoreleased_value;
  int last_drained_autorelease_value;
} objc3_runtime_memory_management_state_snapshot;

int objc3_runtime_copy_memory_management_state_for_testing(
    objc3_runtime_memory_management_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
