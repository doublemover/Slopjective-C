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

}  // namespace objc3c::runtime
