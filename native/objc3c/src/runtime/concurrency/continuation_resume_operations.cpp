#include "runtime/concurrency/continuation_resume_operations.h"

#include "runtime/concurrency/continuation_state_store.h"

namespace objc3c::runtime {

namespace {

RuntimeContinuationRecord *FindContinuationRecord(
    RuntimeContinuationState &state,
    int continuation_handle) {
  state.last_observed_slot = 0;
  state.last_observed_generation = 0;

  if (continuation_handle <= 0) {
    state.last_operation_failure_code =
        kRuntimeContinuationFailureMalformedHandle;
    return nullptr;
  }

  const int slot = RuntimeContinuationSlotFromHandle(continuation_handle);
  const int generation =
      RuntimeContinuationGenerationFromHandle(continuation_handle);
  state.last_observed_slot = slot;
  state.last_observed_generation = generation;

  if (slot <= 0 || generation < 0) {
    state.last_operation_failure_code =
        kRuntimeContinuationFailureMalformedHandle;
    return nullptr;
  }

  const auto found = state.continuation_slots.find(slot);
  if (found == state.continuation_slots.end()) {
    state.last_operation_failure_code = kRuntimeContinuationFailureUnknownHandle;
    return nullptr;
  }

  RuntimeContinuationRecord &record = found->second;
  if (record.generation != generation || record.handle != continuation_handle) {
    state.last_operation_failure_code =
        kRuntimeContinuationFailureStaleGeneration;
    return nullptr;
  }

  if (record.lifecycle_state == RuntimeContinuationLifecycleState::kCompleted) {
    state.last_operation_failure_code =
        kRuntimeContinuationFailureAlreadyCompleted;
    return nullptr;
  }
  if (record.lifecycle_state == RuntimeContinuationLifecycleState::kCancelled) {
    state.last_operation_failure_code =
        kRuntimeContinuationFailureAlreadyCancelled;
    return nullptr;
  }
  if (record.lifecycle_state == RuntimeContinuationLifecycleState::kFailed) {
    state.last_operation_failure_code = kRuntimeContinuationFailureAlreadyFailed;
    return nullptr;
  }

  state.last_operation_failure_code = kRuntimeContinuationFailureNone;
  return &record;
}

void RecycleCompletedContinuation(RuntimeContinuationState &state,
                                  const RuntimeContinuationRecord &record) {
  state.reusable_slots.insert(record.slot);
}

int RejectContinuationOperation(RuntimeContinuationState &state) {
  ++state.rejected_operation_count;
  return 0;
}

}  // namespace

int HandoffRuntimeAsyncContinuationToExecutor(
    RuntimeContinuationState &state,
    int continuation_handle,
    int executor_tag) {
  ++state.handoff_call_count;
  state.last_handoff_handle = continuation_handle;
  state.last_handoff_executor_tag = executor_tag;
  RuntimeContinuationRecord *record =
      FindContinuationRecord(state, continuation_handle);
  if (record == nullptr) {
    return RejectContinuationOperation(state);
  }
  if (executor_tag <= 0) {
    state.last_operation_failure_code =
        kRuntimeContinuationFailureInvalidExecutor;
    return RejectContinuationOperation(state);
  }
  record->handoff_executor_tag = executor_tag;
  record->lifecycle_state = RuntimeContinuationLifecycleState::kHandedOff;
  return continuation_handle;
}

int ResumeRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                   int continuation_handle,
                                   int result_value) {
  ++state.resume_call_count;
  state.last_resume_handle = continuation_handle;
  state.last_resume_result_value = result_value;
  RuntimeContinuationRecord *record =
      FindContinuationRecord(state, continuation_handle);
  if (record == nullptr) {
    state.last_resume_return_value = 0;
    return RejectContinuationOperation(state);
  }
  if (result_value == 0) {
    record->lifecycle_state = RuntimeContinuationLifecycleState::kFailed;
    ++state.failed_continuation_count;
    state.last_resume_return_value = 0;
    state.last_operation_failure_code =
        kRuntimeContinuationFailureMissingPayload;
    RecycleCompletedContinuation(state, *record);
    return RejectContinuationOperation(state);
  }

  record->result_value = result_value;
  record->lifecycle_state = RuntimeContinuationLifecycleState::kCompleted;
  ++state.completed_continuation_count;
  state.last_resume_return_value = result_value;
  RecycleCompletedContinuation(state, *record);
  return result_value;
}

int CancelRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                   int continuation_handle) {
  ++state.cancel_call_count;
  state.last_cancel_handle = continuation_handle;
  RuntimeContinuationRecord *record =
      FindContinuationRecord(state, continuation_handle);
  if (record == nullptr) {
    state.last_cancel_return_value = 0;
    return RejectContinuationOperation(state);
  }

  record->lifecycle_state = RuntimeContinuationLifecycleState::kCancelled;
  ++state.cancelled_continuation_count;
  state.last_cancel_return_value = continuation_handle;
  RecycleCompletedContinuation(state, *record);
  return continuation_handle;
}

}  // namespace objc3c::runtime
