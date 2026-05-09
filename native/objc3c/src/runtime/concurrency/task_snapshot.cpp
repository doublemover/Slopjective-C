#include "runtime/concurrency/task_state_store.h"

#include "runtime/concurrency/task_snapshot_fields.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::RuntimeTaskState &state =
      objc3c::runtime::RuntimeTaskStateForCurrentThread();
  objc3c::runtime::ResetRuntimeTaskRuntimeStateSnapshot(*snapshot);
  objc3c::runtime::PopulateRuntimeTaskRuntimeStateSnapshot(
      state, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
