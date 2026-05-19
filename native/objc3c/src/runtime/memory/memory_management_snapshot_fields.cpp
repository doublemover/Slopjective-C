#include "runtime/memory/memory_management_snapshot_fields.h"

#include "runtime/memory/autorelease_pool.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>

namespace objc3c::runtime {

void ResetRuntimeMemoryManagementStateSnapshot(
    objc3_runtime_memory_management_state_snapshot &snapshot) {
  snapshot.live_runtime_instance_count = 0;
  snapshot.weak_target_count = 0;
  snapshot.weak_slot_ref_count = 0;
  snapshot.autoreleasepool_depth = 0;
  snapshot.autoreleasepool_max_depth = 0;
  snapshot.queued_autorelease_value_count = 0;
  snapshot.drained_autorelease_value_count = 0;
  snapshot.last_autoreleased_value = 0;
  snapshot.last_drained_autorelease_value = 0;
}

void PopulateRuntimeAutoreleaseSnapshotFields(
    objc3_runtime_memory_management_state_snapshot &snapshot) {
  snapshot.autoreleasepool_depth = RuntimeAutoreleasePoolDepth();
  snapshot.autoreleasepool_max_depth = RuntimeAutoreleasePoolMaxDepth();
  snapshot.queued_autorelease_value_count = CountQueuedAutoreleaseValues();
  snapshot.drained_autorelease_value_count =
      RuntimeAutoreleasePoolDrainedValueCount();
  snapshot.last_autoreleased_value = RuntimeLastAutoreleasedValue();
  snapshot.last_drained_autorelease_value =
      RuntimeLastDrainedAutoreleaseValue();
}

void PopulateRuntimeMemoryManagementSnapshotFieldsUnlocked(
    const RuntimeState &state,
    objc3_runtime_memory_management_state_snapshot &snapshot) {
  snapshot.live_runtime_instance_count = state.live_runtime_instance_count;
  snapshot.weak_target_count =
      static_cast<std::uint64_t>(state.weak_slot_refs_by_target_receiver.size());
  for (const auto &entry : state.weak_slot_refs_by_target_receiver) {
    snapshot.weak_slot_ref_count +=
        static_cast<std::uint64_t>(entry.second.size());
  }
}

}  // namespace objc3c::runtime
