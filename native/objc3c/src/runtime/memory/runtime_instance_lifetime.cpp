#include "runtime/memory/runtime_instance_lifetime.h"

#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/memory/runtime_instance_destroy_plan.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/weak_slots.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace objc3c::runtime {

void DestroyRuntimeInstanceUnlocked(RuntimeState &state, int receiver) {
  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    return;
  }
  RuntimeInstanceRecord instance = std::move(instance_it->second);
  state.runtime_instances_by_receiver.erase(instance_it);
  state.live_runtime_instance_count =
      static_cast<std::uint64_t>(state.runtime_instances_by_receiver.size());

  ZeroWeakSlotRefsForTargetUnlocked(state, receiver);
  RemoveWeakSlotRefsOwnedByReceiverUnlocked(state, receiver);

  const std::vector<int> owned_values_to_release =
      RuntimeInstanceOwnedValuesToReleaseUnlocked(state, instance);
  for (int stored_value : owned_values_to_release) {
    ReleaseRuntimeValueUnlocked(state, stored_value);
  }
}

}  // namespace objc3c::runtime
