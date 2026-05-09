#include "runtime/storage/property_accessors.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/reflection/property_entry_snapshot_fields.h"
#include "runtime/reflection/property_reflection_query_state.h"
#include "runtime/reflection/property_registry_snapshot_fields.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstddef>
#include <mutex>

extern "C" int objc3_runtime_copy_property_registry_state_for_testing(
    objc3_runtime_property_registry_state_snapshot *snapshot) {
  // property-metadata-reflection anchor: the private registry-state snapshot is
  // the canonical diagnostic/testing surface for aggregate reflectable
  // property metadata counts and last-query evidence.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::ResetRuntimePropertyRegistryStateSnapshot(*snapshot);

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::PopulateRuntimePropertyRegistryStateSnapshotUnlocked(
      state, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_property_entry_for_testing(
    const char *class_name,
    const char *property_name,
    objc3_runtime_property_entry_snapshot *snapshot) {
  // property-metadata-reflection anchor: the private per-property snapshot
  // exposes runtime-owned accessor/layout facts by class/property name without
  // widening the public ABI or synthesizing metadata from source.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::ResetRuntimePropertyEntrySnapshot(*snapshot);

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::BeginRuntimePropertyReflectionQueryUnlocked(
      state, class_name, property_name);

  snapshot->queried_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_queried_property_class_name);
  if (class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const objc3c::runtime::RealizedClassNode &start_node =
      state.realized_class_nodes[node_index];
  const objc3c::runtime::RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const objc3c::runtime::RealizedPropertyAccessor *accessor =
      objc3c::runtime::FindRuntimePropertyAccessorByNameUnlocked(
          state, start_node, property_name, resolved_node, inherited,
          used_cache);
  objc3c::runtime::RecordRuntimePropertyReflectionCacheUseUnlocked(
      state, used_cache);
  if (accessor == nullptr || resolved_node == nullptr ||
      accessor->property_descriptor == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  objc3c::runtime::RecordRuntimePropertyReflectionHitUnlocked(
      state, *resolved_node, *accessor, inherited);

  objc3c::runtime::PopulateRuntimePropertyEntrySnapshotUnlocked(
      state, *resolved_node, *accessor, inherited, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
