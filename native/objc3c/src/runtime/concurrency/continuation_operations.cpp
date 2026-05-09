#include "runtime/concurrency/continuation_operations.h"

#include "runtime/concurrency/continuation_state_store.h"

namespace objc3c::runtime {

int AllocateRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                     int resume_entry_tag,
                                     int executor_tag) {
  ++state.allocation_call_count;
  const int handle = state.next_handle++;
  state.last_allocated_handle = handle;
  state.last_allocated_resume_entry_tag = resume_entry_tag;
  state.last_allocated_executor_tag = executor_tag;
  state.live_handles.insert(handle);
  return handle;
}

int HandoffRuntimeAsyncContinuationToExecutor(
    RuntimeContinuationState &state,
    int continuation_handle,
    int executor_tag) {
  ++state.handoff_call_count;
  state.last_handoff_handle = continuation_handle;
  state.last_handoff_executor_tag = executor_tag;
  if (continuation_handle == 0 ||
      state.live_handles.find(continuation_handle) ==
          state.live_handles.end()) {
    return 0;
  }
  return continuation_handle;
}

int ResumeRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                   int continuation_handle,
                                   int result_value) {
  ++state.resume_call_count;
  state.last_resume_handle = continuation_handle;
  state.last_resume_result_value = result_value;
  const auto found = state.live_handles.find(continuation_handle);
  if (continuation_handle == 0 || found == state.live_handles.end()) {
    state.last_resume_return_value = 0;
    return 0;
  }
  state.live_handles.erase(found);
  state.last_resume_return_value = result_value;
  return result_value;
}

}  // namespace objc3c::runtime
