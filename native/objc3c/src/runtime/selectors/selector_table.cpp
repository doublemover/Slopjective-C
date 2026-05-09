#include "runtime/selectors/selector_table.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/selectors/selector_snapshot.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

extern "C" const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector) {
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::LookupSelectorUnlocked(selector);
}

extern "C" int objc3_runtime_copy_selector_lookup_table_state_for_testing(
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::CopySelectorLookupTableStateSnapshotUnlocked(state,
                                                                snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_selector_lookup_entry_for_testing(
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::CopySelectorLookupEntrySnapshotUnlocked(state, selector,
                                                           snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
