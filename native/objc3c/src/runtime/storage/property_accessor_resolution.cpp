#include "runtime/storage/property_accessor_resolution.h"

#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessor_dispatch_record.h"

#include <unordered_set>

namespace objc3c::runtime {

bool TryResolveRuntimeManagedPropertyAccessorUnlocked(
    const RuntimeState &state, const RealizedClassNode &start_node,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    SlowPathResolution &resolution) {
  if (selector_spelling == nullptr || selector_spelling[0] == '\0') {
    return false;
  }
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (TryResolveRuntimePropertyAccessorOnNode(
            *node, normalized_receiver_identity, selector_stable_id,
            selector_spelling, resolution)) {
      return true;
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return false;
}

}  // namespace objc3c::runtime
