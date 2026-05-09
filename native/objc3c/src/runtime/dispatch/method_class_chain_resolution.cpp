#include "runtime/dispatch/method_class_chain_resolution.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/method_resolution_tables.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"

#include <unordered_set>

namespace objc3c::runtime {

bool ResolveReceiverClassNameUnlocked(const RuntimeState &state,
                                      std::uint64_t base_identity,
                                      std::string &class_name,
                                      bool &ambiguous) {
  ambiguous = false;
  if (!IsRuntimeReceiverBaseIdentity(base_identity)) {
    return false;
  }
  if (state.ambiguous_realized_base_identities.find(base_identity) !=
      state.ambiguous_realized_base_identities.end()) {
    ambiguous = true;
    return false;
  }
  const auto found =
      state.realized_class_name_by_base_identity.find(base_identity);
  if (found == state.realized_class_name_by_base_identity.end()) {
    return false;
  }
  class_name = found->second;
  return true;
}

bool TryResolveMethodFromRealizedClassChainUnlocked(
    RuntimeState &state, const RealizedClassNode *start_node,
    DispatchFamily family, std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    SlowPathResolution &resolution, bool &ambiguous,
    std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count) {
  // class-realization-runtime freeze anchor: runtime walks the emitted
  // class/metaclass chain and attached categories published by class graph
  // realization.
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->bundle == nullptr || node->image == nullptr) {
      return false;
    }
    const EmittedClassBundle *bundle = node->bundle;
    const EmittedClassRecord &record =
        family == DispatchFamily::Class ? bundle->metaclass_record
                                        : bundle->class_record;
    if (!TryResolveMethodFromMethodListRefUnlocked(
            state, record.method_list_ref, record.class_name, family,
            normalized_receiver_identity, selector_stable_id, selector_spelling,
            resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
      return true;
    }
    if (!TryResolveMethodFromAttachedCategoriesUnlocked(
            state, *node, family, normalized_receiver_identity,
            selector_stable_id, selector_spelling, resolution, ambiguous,
            category_probe_count, protocol_probe_count)) {
      return false;
    }
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
      return true;
    }
    if (!ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
            state, record.adopted_protocol_refs, family, selector_stable_id,
            selector_spelling, protocol_probe_count, ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return true;
}

}  // namespace objc3c::runtime
