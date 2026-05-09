#pragma once

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeTaskState {
  std::uint64_t spawn_call_count = 0;
  std::uint64_t scope_call_count = 0;
  std::uint64_t add_task_call_count = 0;
  std::uint64_t wait_next_call_count = 0;
  std::uint64_t cancel_all_call_count = 0;
  std::uint64_t cancellation_poll_call_count = 0;
  std::uint64_t on_cancel_call_count = 0;
  std::uint64_t executor_hop_call_count = 0;
  int last_spawn_kind = 0;
  int last_spawn_executor_tag = 0;
  int last_scope_executor_tag = 0;
  int last_add_task_executor_tag = 0;
  int last_wait_next_executor_tag = 0;
  int last_cancel_all_executor_tag = 0;
  int last_cancellation_poll_executor_tag = 0;
  int last_on_cancel_executor_tag = 0;
  int last_executor_hop_executor_tag = 0;
  int last_executor_hop_value = 0;
  int last_wait_next_result = 0;
  int last_cancel_all_result = 0;
  int last_cancellation_poll_result = 0;
};

RuntimeTaskState &RuntimeTaskStateForCurrentThread();

}  // namespace objc3c::runtime
