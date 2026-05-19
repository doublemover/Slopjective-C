#include "runtime/concurrency/continuation_snapshot_fields.h"

#include "runtime/concurrency/continuation_state_store.h"

namespace objc3c::runtime {

void ResetRuntimeAsyncContinuationStateSnapshot(
    objc3_runtime_async_continuation_state_snapshot &snapshot) {
  snapshot.allocation_call_count = 0;
  snapshot.handoff_call_count = 0;
  snapshot.resume_call_count = 0;
  snapshot.live_continuation_handle_count = 0;
  snapshot.last_allocated_continuation_handle = 0;
  snapshot.last_allocated_resume_entry_tag = 0;
  snapshot.last_allocated_executor_tag = 0;
  snapshot.last_handoff_continuation_handle = 0;
  snapshot.last_handoff_executor_tag = 0;
  snapshot.last_resume_continuation_handle = 0;
  snapshot.last_resume_result_value = 0;
  snapshot.last_resume_return_value = 0;
}

void PopulateRuntimeAsyncContinuationStateSnapshot(
    const RuntimeContinuationState &state,
    objc3_runtime_async_continuation_state_snapshot &snapshot) {
  snapshot.allocation_call_count = state.allocation_call_count;
  snapshot.handoff_call_count = state.handoff_call_count;
  snapshot.resume_call_count = state.resume_call_count;
  snapshot.live_continuation_handle_count = state.live_handles.size();
  snapshot.last_allocated_continuation_handle = state.last_allocated_handle;
  snapshot.last_allocated_resume_entry_tag =
      state.last_allocated_resume_entry_tag;
  snapshot.last_allocated_executor_tag = state.last_allocated_executor_tag;
  snapshot.last_handoff_continuation_handle = state.last_handoff_handle;
  snapshot.last_handoff_executor_tag = state.last_handoff_executor_tag;
  snapshot.last_resume_continuation_handle = state.last_resume_handle;
  snapshot.last_resume_result_value = state.last_resume_result_value;
  snapshot.last_resume_return_value = state.last_resume_return_value;
}

}  // namespace objc3c::runtime
