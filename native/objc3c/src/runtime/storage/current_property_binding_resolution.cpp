#include "runtime/storage/current_property_binding_resolution.h"

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/reflection/property_reflection_query_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_accessors.h"

#include <cstddef>

namespace objc3c::runtime {

RuntimeCurrentPropertyBindingResolution ResolveCurrentPropertyBindingUnlocked(
    RuntimeState &state,
    int receiver,
    const char *class_name,
    const char *property_name) {
  RuntimeCurrentPropertyBindingResolution resolution;
  resolution.status = OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  if (receiver == 0 || class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return resolution;
  }

  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (instance_it == state.runtime_instances_by_receiver.end() ||
      found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return resolution;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return resolution;
  }

  const RealizedClassNode &start_node = state.realized_class_nodes[node_index];
  const RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const RealizedPropertyAccessor *accessor =
      FindRuntimePropertyAccessorByNameUnlocked(
          state, start_node, property_name, resolved_node, inherited,
          used_cache);
  RecordRuntimePropertyReflectionCacheUseUnlocked(state, used_cache);
  if (accessor == nullptr || resolved_node == nullptr) {
    return resolution;
  }

  resolution.status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  resolution.base_identity = instance_it->second.base_identity;
  resolution.accessor = accessor;
  return resolution;
}

}  // namespace objc3c::runtime
