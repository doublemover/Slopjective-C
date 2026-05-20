#pragma once

#include <cstdint>

namespace objc3c::runtime {

inline constexpr int kRuntimeTaskFailureNone = 0;
inline constexpr int kRuntimeTaskFailureInvalidExecutor = 1;
inline constexpr int kRuntimeTaskFailureUnsupportedTaskKind = 2;
inline constexpr int kRuntimeTaskFailureMissingTaskGroup = 3;
inline constexpr int kRuntimeTaskFailureTaskGroupAlreadyActive = 4;
inline constexpr int kRuntimeTaskFailureExecutorMismatch = 5;
inline constexpr int kRuntimeTaskFailureEmptyTaskGroupQueue = 6;
inline constexpr int kRuntimeTaskFailureTaskGroupAlreadyCancelled = 7;
inline constexpr int kRuntimeTaskLifecycleIdle = 0;
inline constexpr int kRuntimeTaskLifecycleTaskSpawned = 1;
inline constexpr int kRuntimeTaskLifecycleGroupActive = 2;
inline constexpr int kRuntimeTaskLifecycleGroupDrained = 3;
inline constexpr int kRuntimeTaskLifecycleGroupCancelled = 4;

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
  int last_failure_reason = kRuntimeTaskFailureNone;
  int lifecycle_state = kRuntimeTaskLifecycleIdle;
  int selected_executor_tag = 0;
  int active_group_executor_tag = -1;
  int active_group_task_count = 0;
  int pending_group_task_count = 0;
  int completed_group_task_count = 0;
  int group_cancelled = 0;
  int cancellation_generation = 0;
  int observed_cancellation_generation = 0;
  int last_queue_depth = 0;
  int last_queue_drain_result = 0;
};

RuntimeTaskState &RuntimeTaskStateForCurrentThread();

}  // namespace objc3c::runtime
