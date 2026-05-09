#include "runtime/selectors/keypath_table.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/selectors/keypath_records.h"
#include "runtime/selectors/keypath_snapshot.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

namespace objc3c::runtime {

bool MaterializeKeyPathDescriptorUnlocked(
    RuntimeState &state,
    const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal) {
  if (!RuntimeKeyPathDescriptorIsComplete(descriptor)) {
    return false;
  }

  KeyPathSlot *slot =
      FindRuntimeKeyPathSlotUnlocked(state, descriptor.stable_id);
  if (slot == nullptr) {
    return InsertImageBackedRuntimeKeyPathSlotUnlocked(
        state, descriptor, registration_order_ordinal);
  }

  MergeImageBackedRuntimeKeyPathSlotUnlocked(
      state, *slot, descriptor, registration_order_ordinal);
  return true;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_copy_keypath_registry_state_for_testing(
    objc3_runtime_keypath_registry_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::CopyRuntimeKeyPathRegistryStateSnapshotUnlocked(state,
                                                                   snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_keypath_entry_for_testing(
    std::uint64_t stable_id,
    objc3_runtime_keypath_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::CopyRuntimeKeyPathEntrySnapshotUnlocked(state, stable_id,
                                                           snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_keypath_component_count_for_testing(
    int keypath_handle) {
  if (keypath_handle <= 0 ||
      !objc3c::runtime::RuntimeKeyPathHandleIsValid(
          static_cast<std::uint64_t>(keypath_handle))) {
    return 0;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::RuntimeKeyPathComponentCountForTestingUnlocked(
      state, static_cast<std::uint64_t>(keypath_handle));
}

extern "C" int objc3_runtime_keypath_root_is_self_for_testing(
    int keypath_handle) {
  if (keypath_handle <= 0 ||
      !objc3c::runtime::RuntimeKeyPathHandleIsValid(
          static_cast<std::uint64_t>(keypath_handle))) {
    return 0;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::RuntimeKeyPathRootIsSelfForTestingUnlocked(
      state, static_cast<std::uint64_t>(keypath_handle));
}
