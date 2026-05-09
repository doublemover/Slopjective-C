#include "runtime/memory/runtime_instance_lifetime.h"

#include "runtime/classes/class_graph.h"
#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessors.h"
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

  const RealizedClassNode *node =
      FindRealizedClassNodeByBaseIdentityUnlocked(state,
                                                  instance.base_identity);
  std::vector<int> owned_values_to_release;
  if (node != nullptr && node->runtime_layout_ready) {
    owned_values_to_release.reserve(node->runtime_property_accessors.size());
    for (const RealizedPropertyAccessor &accessor :
         node->runtime_property_accessors) {
      if (!UsesStrongOwnedRuntimeHooks(accessor)) {
        continue;
      }
      int stored_value = 0;
      if (ReadRuntimeManagedPropertyValueRaw(instance, accessor,
                                             stored_value) &&
          stored_value != 0) {
        owned_values_to_release.push_back(stored_value);
      }
    }
  }
  for (int stored_value : owned_values_to_release) {
    ReleaseRuntimeValueUnlocked(state, stored_value);
  }
}

}  // namespace objc3c::runtime
