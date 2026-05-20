#pragma once

#include <cstdint>
#include <set>
#include <unordered_map>

namespace objc3c::runtime {

constexpr int kRuntimeContinuationSlotBits = 16;
constexpr int kRuntimeContinuationSlotMask =
    (1 << kRuntimeContinuationSlotBits) - 1;
constexpr int kRuntimeContinuationMaxGeneration =
    (1 << (31 - kRuntimeContinuationSlotBits)) - 1;

enum RuntimeContinuationFailureCode {
  kRuntimeContinuationFailureNone = 0,
  kRuntimeContinuationFailureMalformedHandle = 1,
  kRuntimeContinuationFailureUnknownHandle = 2,
  kRuntimeContinuationFailureStaleGeneration = 3,
  kRuntimeContinuationFailureAlreadyCompleted = 4,
  kRuntimeContinuationFailureAlreadyCancelled = 5,
  kRuntimeContinuationFailureMissingPayload = 6,
  kRuntimeContinuationFailureInvalidAllocationPayload = 7,
  kRuntimeContinuationFailureInvalidExecutor = 8,
  kRuntimeContinuationFailureAlreadyFailed = 9,
  kRuntimeContinuationFailureSlotExhausted = 10,
  kRuntimeContinuationFailureNotHandedOff = 11,
  kRuntimeContinuationFailureExecutorMismatch = 12,
};

enum class RuntimeContinuationLifecycleState {
  kAllocated,
  kHandedOff,
  kCompleted,
  kCancelled,
  kFailed,
};

struct RuntimeContinuationRecord {
  int slot = 0;
  int generation = 0;
  int handle = 0;
  int resume_entry_tag = 0;
  int executor_tag = 0;
  int handoff_executor_tag = 0;
  int result_value = 0;
  RuntimeContinuationLifecycleState lifecycle_state =
      RuntimeContinuationLifecycleState::kFailed;
};

struct RuntimeContinuationState {
  std::uint64_t allocation_call_count = 0;
  std::uint64_t handoff_call_count = 0;
  std::uint64_t resume_call_count = 0;
  std::uint64_t cancel_call_count = 0;
  std::uint64_t rejected_operation_count = 0;
  std::uint64_t completed_continuation_count = 0;
  std::uint64_t cancelled_continuation_count = 0;
  std::uint64_t failed_continuation_count = 0;
  int next_slot = 1;
  int last_allocated_handle = 0;
  int last_allocated_slot = 0;
  int last_allocated_generation = 0;
  int last_allocated_resume_entry_tag = 0;
  int last_allocated_executor_tag = 0;
  int last_handoff_handle = 0;
  int last_handoff_executor_tag = 0;
  int last_resume_handle = 0;
  int last_resume_result_value = 0;
  int last_resume_return_value = 0;
  int last_cancel_handle = 0;
  int last_cancel_return_value = 0;
  int last_operation_failure_code = kRuntimeContinuationFailureNone;
  int last_observed_slot = 0;
  int last_observed_generation = 0;
  std::unordered_map<int, RuntimeContinuationRecord> continuation_slots;
  std::set<int> reusable_slots;
};

RuntimeContinuationState &RuntimeContinuationStateForCurrentThread();

inline int RuntimeContinuationHandleForSlotGeneration(int slot,
                                                      int generation) {
  return (generation << kRuntimeContinuationSlotBits) | slot;
}

inline int RuntimeContinuationSlotFromHandle(int handle) {
  return handle & kRuntimeContinuationSlotMask;
}

inline int RuntimeContinuationGenerationFromHandle(int handle) {
  return handle >> kRuntimeContinuationSlotBits;
}

inline bool RuntimeContinuationLifecycleStateIsLive(
    RuntimeContinuationLifecycleState lifecycle_state) {
  return lifecycle_state == RuntimeContinuationLifecycleState::kAllocated ||
         lifecycle_state == RuntimeContinuationLifecycleState::kHandedOff;
}

}  // namespace objc3c::runtime
