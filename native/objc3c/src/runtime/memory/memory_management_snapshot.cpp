#include "runtime/objc3_runtime_bootstrap_internal.h"

#include "runtime/memory/autorelease_pool.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstdint>
#include <mutex>

extern "C" int objc3_runtime_copy_memory_management_state_for_testing(
    objc3_runtime_memory_management_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->live_runtime_instance_count = 0;
  snapshot->weak_target_count = 0;
  snapshot->weak_slot_ref_count = 0;
  snapshot->autoreleasepool_depth =
      objc3c::runtime::RuntimeAutoreleasePoolDepth();
  snapshot->autoreleasepool_max_depth =
      objc3c::runtime::RuntimeAutoreleasePoolMaxDepth();
  snapshot->queued_autorelease_value_count =
      objc3c::runtime::CountQueuedAutoreleaseValues();
  snapshot->drained_autorelease_value_count =
      objc3c::runtime::RuntimeAutoreleasePoolDrainedValueCount();
  snapshot->last_autoreleased_value =
      objc3c::runtime::RuntimeLastAutoreleasedValue();
  snapshot->last_drained_autorelease_value =
      objc3c::runtime::RuntimeLastDrainedAutoreleaseValue();

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->live_runtime_instance_count = state.live_runtime_instance_count;
  snapshot->weak_target_count =
      static_cast<std::uint64_t>(state.weak_slot_refs_by_target_receiver.size());
  for (const auto &entry : state.weak_slot_refs_by_target_receiver) {
    snapshot->weak_slot_ref_count +=
        static_cast<std::uint64_t>(entry.second.size());
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
