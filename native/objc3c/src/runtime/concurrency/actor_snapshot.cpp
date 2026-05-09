#include "runtime/concurrency/actor_state_store.h"

#include "runtime/concurrency/actor_snapshot_fields.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  objc3c::runtime::ResetRuntimeActorRuntimeStateSnapshot(*snapshot);
  objc3c::runtime::PopulateRuntimeActorRuntimeStateSnapshot(
      state, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
