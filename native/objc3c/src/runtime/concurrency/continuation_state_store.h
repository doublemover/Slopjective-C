#pragma once

#include <cstdint>
#include <unordered_set>

namespace objc3c::runtime {

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

RuntimeContinuationState &RuntimeContinuationStateForCurrentThread();

}  // namespace objc3c::runtime
