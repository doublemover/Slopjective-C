#include "runtime/storage/property_accessors.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/reflection/property_entry_snapshot_fields.h"
#include "runtime/reflection/property_entry_query.h"
#include "runtime/reflection/property_registry_snapshot_fields.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

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
  objc3c::runtime::CopyRuntimePropertyEntrySnapshotForQueryUnlocked(
      state, class_name, property_name, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
