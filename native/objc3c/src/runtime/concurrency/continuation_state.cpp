#include "runtime/concurrency/continuation_state.h"

#include "runtime/concurrency/continuation_operations.h"
#include "runtime/concurrency/continuation_resume_operations.h"
#include "runtime/concurrency/continuation_snapshot_fields.h"
#include "runtime/concurrency/continuation_state_store.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_allocate_async_continuation_i32(
    int resume_entry_tag, int executor_tag) {
  objc3c::runtime::RuntimeContinuationState &state =
      objc3c::runtime::RuntimeContinuationStateForCurrentThread();
  return objc3c::runtime::AllocateRuntimeAsyncContinuation(
      state, resume_entry_tag, executor_tag);
}

extern "C" int objc3_runtime_handoff_async_continuation_to_executor_i32(
    int continuation_handle, int executor_tag) {
  objc3c::runtime::RuntimeContinuationState &state =
      objc3c::runtime::RuntimeContinuationStateForCurrentThread();
  return objc3c::runtime::HandoffRuntimeAsyncContinuationToExecutor(
      state, continuation_handle, executor_tag);
}

extern "C" int objc3_runtime_resume_async_continuation_i32(
    int continuation_handle, int result_value) {
  objc3c::runtime::RuntimeContinuationState &state =
      objc3c::runtime::RuntimeContinuationStateForCurrentThread();
  return objc3c::runtime::ResumeRuntimeAsyncContinuation(
      state, continuation_handle, result_value);
}

extern "C" int objc3_runtime_copy_async_continuation_state_for_testing(
    objc3_runtime_async_continuation_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::RuntimeContinuationState &state =
      objc3c::runtime::RuntimeContinuationStateForCurrentThread();
  objc3c::runtime::ResetRuntimeAsyncContinuationStateSnapshot(*snapshot);
  objc3c::runtime::PopulateRuntimeAsyncContinuationStateSnapshot(
      state, *snapshot);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
