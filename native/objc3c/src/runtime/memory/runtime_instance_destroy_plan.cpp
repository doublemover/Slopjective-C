#include "runtime/memory/runtime_instance_destroy_plan.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessors.h"

namespace objc3c::runtime {

std::vector<int> RuntimeInstanceOwnedValuesToReleaseUnlocked(
    RuntimeState &state,
    const RuntimeInstanceRecord &instance) {
  const RealizedClassNode *node =
      FindRealizedClassNodeByBaseIdentityUnlocked(state,
                                                  instance.base_identity);
  std::vector<int> owned_values_to_release;
  if (node == nullptr || !node->runtime_layout_ready) {
    return owned_values_to_release;
  }

  owned_values_to_release.reserve(node->runtime_property_accessors.size());
  for (const RealizedPropertyAccessor &accessor :
       node->runtime_property_accessors) {
    if (!UsesStrongOwnedRuntimeHooks(accessor)) {
      continue;
    }
    int stored_value = 0;
    if (ReadRuntimeManagedPropertyValueRaw(instance, accessor, stored_value) &&
        stored_value != 0) {
      owned_values_to_release.push_back(stored_value);
    }
  }
  return owned_values_to_release;
}

}  // namespace objc3c::runtime
