#include "runtime/concurrency/continuation_operations.h"

#include "runtime/concurrency/continuation_state_store.h"

namespace objc3c::runtime {

int AllocateRuntimeAsyncContinuation(RuntimeContinuationState &state,
                                     int resume_entry_tag,
                                     int executor_tag) {
  ++state.allocation_call_count;
  state.last_allocated_resume_entry_tag = resume_entry_tag;
  state.last_allocated_executor_tag = executor_tag;
  state.last_operation_failure_code = kRuntimeContinuationFailureNone;

  if (resume_entry_tag <= 0 || executor_tag <= 0) {
    ++state.rejected_operation_count;
    state.last_allocated_handle = 0;
    state.last_allocated_slot = 0;
    state.last_allocated_generation = 0;
    state.last_operation_failure_code =
        kRuntimeContinuationFailureInvalidAllocationPayload;
    return 0;
  }

  int slot = 0;
  int generation = 0;
  if (!state.reusable_slots.empty()) {
    slot = *state.reusable_slots.begin();
    state.reusable_slots.erase(state.reusable_slots.begin());
    RuntimeContinuationRecord &record = state.continuation_slots[slot];
    generation = record.generation + 1;
    if (generation > kRuntimeContinuationMaxGeneration) {
      ++state.rejected_operation_count;
      state.last_allocated_handle = 0;
      state.last_allocated_slot = slot;
      state.last_allocated_generation = record.generation;
      state.last_operation_failure_code =
          kRuntimeContinuationFailureSlotExhausted;
      return 0;
    }
  } else {
    slot = state.next_slot++;
    if (slot <= 0 || slot > kRuntimeContinuationSlotMask) {
      ++state.rejected_operation_count;
      state.last_allocated_handle = 0;
      state.last_allocated_slot = 0;
      state.last_allocated_generation = 0;
      state.last_operation_failure_code =
          kRuntimeContinuationFailureSlotExhausted;
      return 0;
    }
  }

  const int handle =
      RuntimeContinuationHandleForSlotGeneration(slot, generation);
  RuntimeContinuationRecord record{};
  record.slot = slot;
  record.generation = generation;
  record.handle = handle;
  record.resume_entry_tag = resume_entry_tag;
  record.executor_tag = executor_tag;
  record.lifecycle_state = RuntimeContinuationLifecycleState::kAllocated;

  state.continuation_slots[slot] = record;
  state.last_allocated_handle = handle;
  state.last_allocated_slot = slot;
  state.last_allocated_generation = generation;
  return handle;
}

}  // namespace objc3c::runtime
