#include "runtime/concurrency/continuation_snapshot_fields.h"

#include "runtime/concurrency/continuation_state_store.h"

namespace objc3c::runtime {

void ResetRuntimeAsyncContinuationStateSnapshot(
    objc3_runtime_async_continuation_state_snapshot &snapshot) {
  snapshot.allocation_call_count = 0;
  snapshot.handoff_call_count = 0;
  snapshot.resume_call_count = 0;
  snapshot.cancel_call_count = 0;
  snapshot.rejected_operation_count = 0;
  snapshot.live_continuation_handle_count = 0;
  snapshot.completed_continuation_count = 0;
  snapshot.cancelled_continuation_count = 0;
  snapshot.failed_continuation_count = 0;
  snapshot.last_allocated_continuation_handle = 0;
  snapshot.last_allocated_continuation_slot = 0;
  snapshot.last_allocated_continuation_generation = 0;
  snapshot.last_allocated_resume_entry_tag = 0;
  snapshot.last_allocated_executor_tag = 0;
  snapshot.last_handoff_continuation_handle = 0;
  snapshot.last_handoff_executor_tag = 0;
  snapshot.last_resume_continuation_handle = 0;
  snapshot.last_resume_result_value = 0;
  snapshot.last_resume_return_value = 0;
  snapshot.last_cancel_continuation_handle = 0;
  snapshot.last_cancel_return_value = 0;
  snapshot.last_operation_failure_code = 0;
  snapshot.last_observed_continuation_slot = 0;
  snapshot.last_observed_continuation_generation = 0;
}

void PopulateRuntimeAsyncContinuationStateSnapshot(
    const RuntimeContinuationState &state,
    objc3_runtime_async_continuation_state_snapshot &snapshot) {
  snapshot.allocation_call_count = state.allocation_call_count;
  snapshot.handoff_call_count = state.handoff_call_count;
  snapshot.resume_call_count = state.resume_call_count;
  snapshot.cancel_call_count = state.cancel_call_count;
  snapshot.rejected_operation_count = state.rejected_operation_count;
  snapshot.live_continuation_handle_count = 0;
  for (const auto &entry : state.continuation_slots) {
    if (RuntimeContinuationLifecycleStateIsLive(entry.second.lifecycle_state)) {
      ++snapshot.live_continuation_handle_count;
    }
  }
  snapshot.completed_continuation_count = state.completed_continuation_count;
  snapshot.cancelled_continuation_count = state.cancelled_continuation_count;
  snapshot.failed_continuation_count = state.failed_continuation_count;
  snapshot.last_allocated_continuation_handle = state.last_allocated_handle;
  snapshot.last_allocated_continuation_slot = state.last_allocated_slot;
  snapshot.last_allocated_continuation_generation =
      state.last_allocated_generation;
  snapshot.last_allocated_resume_entry_tag =
      state.last_allocated_resume_entry_tag;
  snapshot.last_allocated_executor_tag = state.last_allocated_executor_tag;
  snapshot.last_handoff_continuation_handle = state.last_handoff_handle;
  snapshot.last_handoff_executor_tag = state.last_handoff_executor_tag;
  snapshot.last_resume_continuation_handle = state.last_resume_handle;
  snapshot.last_resume_result_value = state.last_resume_result_value;
  snapshot.last_resume_return_value = state.last_resume_return_value;
  snapshot.last_cancel_continuation_handle = state.last_cancel_handle;
  snapshot.last_cancel_return_value = state.last_cancel_return_value;
  snapshot.last_operation_failure_code = state.last_operation_failure_code;
  snapshot.last_observed_continuation_slot = state.last_observed_slot;
  snapshot.last_observed_continuation_generation =
      state.last_observed_generation;
}

}  // namespace objc3c::runtime
