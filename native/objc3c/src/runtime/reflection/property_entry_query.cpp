#include "runtime/reflection/property_entry_query.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/reflection/property_entry_snapshot_fields.h"
#include "runtime/reflection/property_reflection_query_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_lookup.h"
#include "runtime/strings/borrowed_string.h"

#include <cstddef>

namespace objc3c::runtime {

void CopyRuntimePropertyEntrySnapshotForQueryUnlocked(
    RuntimeState &state,
    const char *class_name,
    const char *property_name,
    objc3_runtime_property_entry_snapshot &snapshot) {
  BeginRuntimePropertyReflectionQueryUnlocked(state, class_name,
                                              property_name);

  snapshot.queried_class_name =
      BorrowRuntimeCString(state.last_queried_property_class_name);
  if (class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return;
  }

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return;
  }

  const RealizedClassNode &start_node = state.realized_class_nodes[node_index];
  const RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const RealizedPropertyAccessor *accessor =
      FindRuntimePropertyAccessorByNameUnlocked(state, start_node,
                                                property_name, resolved_node,
                                                inherited, used_cache);
  RecordRuntimePropertyReflectionCacheUseUnlocked(state, used_cache);
  if (accessor == nullptr || resolved_node == nullptr ||
      accessor->property_descriptor == nullptr) {
    return;
  }

  RecordRuntimePropertyReflectionHitUnlocked(state, *resolved_node, *accessor,
                                             inherited);

  PopulateRuntimePropertyEntrySnapshotUnlocked(state, *resolved_node, *accessor,
                                               inherited, snapshot);
}

}  // namespace objc3c::runtime
