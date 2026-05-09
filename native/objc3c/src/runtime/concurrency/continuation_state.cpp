#include "runtime/concurrency/continuation_state.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <unordered_set>

namespace objc3c::runtime {
namespace detail {

struct RuntimeContinuationState {
  std::uint64_t allocation_call_count = 0;
  std::uint64_t handoff_call_count = 0;
  std::uint64_t resume_call_count = 0;
  int next_handle = 1;
  int last_allocated_handle = 0;
  int last_allocated_resume_entry_tag = 0;
  int last_allocated_executor_tag = 0;
  int last_handoff_handle = 0;
  int last_handoff_executor_tag = 0;
  int last_resume_handle = 0;
  int last_resume_result_value = 0;
  int last_resume_return_value = 0;
  std::unordered_set<int> live_handles;
};

RuntimeContinuationState &ContinuationStateForCurrentThread() {
  thread_local RuntimeContinuationState state;
  return state;
}

}  // namespace detail

void ResetRuntimeContinuationStateForTesting() {
  detail::ContinuationStateForCurrentThread() = detail::RuntimeContinuationState{};
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_allocate_async_continuation_i32(
    int resume_entry_tag, int executor_tag) {
  objc3c::runtime::detail::RuntimeContinuationState &state =
      objc3c::runtime::detail::ContinuationStateForCurrentThread();
  ++state.allocation_call_count;
  const int handle = state.next_handle++;
  state.last_allocated_handle = handle;
  state.last_allocated_resume_entry_tag = resume_entry_tag;
  state.last_allocated_executor_tag = executor_tag;
  state.live_handles.insert(handle);
  return handle;
}

extern "C" int objc3_runtime_handoff_async_continuation_to_executor_i32(
    int continuation_handle, int executor_tag) {
  objc3c::runtime::detail::RuntimeContinuationState &state =
      objc3c::runtime::detail::ContinuationStateForCurrentThread();
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

extern "C" int objc3_runtime_resume_async_continuation_i32(
    int continuation_handle, int result_value) {
  objc3c::runtime::detail::RuntimeContinuationState &state =
      objc3c::runtime::detail::ContinuationStateForCurrentThread();
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

extern "C" int objc3_runtime_copy_async_continuation_state_for_testing(
    objc3_runtime_async_continuation_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::detail::RuntimeContinuationState &state =
      objc3c::runtime::detail::ContinuationStateForCurrentThread();
  snapshot->allocation_call_count = state.allocation_call_count;
  snapshot->handoff_call_count = state.handoff_call_count;
  snapshot->resume_call_count = state.resume_call_count;
  snapshot->live_continuation_handle_count = state.live_handles.size();
  snapshot->last_allocated_continuation_handle = state.last_allocated_handle;
  snapshot->last_allocated_resume_entry_tag =
      state.last_allocated_resume_entry_tag;
  snapshot->last_allocated_executor_tag = state.last_allocated_executor_tag;
  snapshot->last_handoff_continuation_handle = state.last_handoff_handle;
  snapshot->last_handoff_executor_tag = state.last_handoff_executor_tag;
  snapshot->last_resume_continuation_handle = state.last_resume_handle;
  snapshot->last_resume_result_value = state.last_resume_result_value;
  snapshot->last_resume_return_value = state.last_resume_return_value;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
