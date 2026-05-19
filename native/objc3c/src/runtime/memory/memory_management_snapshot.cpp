#include "runtime/objc3_runtime_bootstrap_internal.h"

#include "runtime/memory/memory_management_snapshot_fields.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

extern "C" int objc3_runtime_copy_memory_management_state_for_testing(
    objc3_runtime_memory_management_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::ResetRuntimeMemoryManagementStateSnapshot(*snapshot);
  objc3c::runtime::PopulateRuntimeAutoreleaseSnapshotFields(*snapshot);

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::PopulateRuntimeMemoryManagementSnapshotFieldsUnlocked(
      state, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
