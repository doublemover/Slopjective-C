#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <unordered_map>

namespace objc3c::runtime {

inline constexpr int kRuntimeTaskFailureNone = 0;
inline constexpr int kRuntimeTaskFailureInvalidExecutor = 1;
inline constexpr int kRuntimeTaskFailureUnsupportedTaskKind = 2;
inline constexpr int kRuntimeTaskFailureMissingTaskGroup = 3;
inline constexpr int kRuntimeTaskFailureTaskGroupAlreadyActive = 4;
inline constexpr int kRuntimeTaskFailureExecutorMismatch = 5;
inline constexpr int kRuntimeTaskFailureEmptyTaskGroupQueue = 6;
inline constexpr int kRuntimeTaskFailureTaskGroupAlreadyCancelled = 7;
inline constexpr int kRuntimeTaskFailureSchedulerQueueDrift = 8;
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
  int cancelled_group_task_count = 0;
  int group_cancelled = 0;
  int cancellation_generation = 0;
  int observed_cancellation_generation = 0;
  int last_queue_depth = 0;
  int last_queue_drain_result = 0;
  std::uint64_t scheduler_enqueue_count = 0;
  std::uint64_t scheduler_dequeue_count = 0;
  std::uint64_t scheduler_cancelled_count = 0;
  int last_scheduled_task_handle = 0;
  int last_scheduled_executor_tag = 0;
  int last_dequeued_task_handle = 0;
  int last_dequeued_executor_tag = 0;
  int last_cancelled_task_handle = 0;
  int last_cancelled_executor_tag = 0;
  int last_executor_queue_depth = 0;
  int max_executor_queue_depth = 0;
  int scheduler_sequence = 0;
  int deadlock_guard_passed = 1;
  int race_guard_passed = 1;
  std::deque<int> active_group_ready_queue;
  std::unordered_map<int, std::deque<int>> executor_ready_queues;
};

inline int RuntimeTaskQueueDepthForExecutor(const RuntimeTaskState &state,
                                            int executor_tag) {
  const auto found = state.executor_ready_queues.find(executor_tag);
  return found == state.executor_ready_queues.end()
             ? 0
             : static_cast<int>(found->second.size());
}

inline bool RuntimeTaskSchedulerCountsAreBalanced(
    const RuntimeTaskState &state) {
  return state.scheduler_enqueue_count >=
         state.scheduler_dequeue_count + state.scheduler_cancelled_count;
}

inline void RecordRuntimeTaskSchedulerEnqueue(RuntimeTaskState &state,
                                              int executor_tag,
                                              int task_handle) {
  std::deque<int> &queue = state.executor_ready_queues[executor_tag];
  queue.push_back(task_handle);
  ++state.scheduler_enqueue_count;
  ++state.scheduler_sequence;
  state.last_scheduled_task_handle = task_handle;
  state.last_scheduled_executor_tag = executor_tag;
  state.last_executor_queue_depth = static_cast<int>(queue.size());
  if (state.last_executor_queue_depth > state.max_executor_queue_depth) {
    state.max_executor_queue_depth = state.last_executor_queue_depth;
  }
  state.deadlock_guard_passed =
      RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
  state.race_guard_passed = task_handle > 0 && executor_tag >= 0 ? 1 : 0;
}

inline int DrainRuntimeTaskSchedulerQueue(RuntimeTaskState &state,
                                          int executor_tag) {
  auto found = state.executor_ready_queues.find(executor_tag);
  if (found == state.executor_ready_queues.end() || found->second.empty()) {
    state.last_dequeued_task_handle = 0;
    state.last_dequeued_executor_tag = executor_tag;
    state.last_executor_queue_depth = 0;
    state.deadlock_guard_passed = 0;
    return 0;
  }

  std::deque<int> &queue = found->second;
  const int task_handle = queue.front();
  queue.pop_front();
  ++state.scheduler_dequeue_count;
  ++state.scheduler_sequence;
  state.last_dequeued_task_handle = task_handle;
  state.last_dequeued_executor_tag = executor_tag;
  state.last_executor_queue_depth = static_cast<int>(queue.size());
  state.deadlock_guard_passed =
      RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
  state.race_guard_passed = task_handle > 0 && executor_tag >= 0 ? 1 : 0;
  return task_handle;
}

inline int DrainRuntimeTaskGroupSchedulerQueue(RuntimeTaskState &state,
                                               int executor_tag) {
  if (state.active_group_ready_queue.empty()) {
    state.last_dequeued_task_handle = 0;
    state.last_dequeued_executor_tag = executor_tag;
    state.last_executor_queue_depth =
        RuntimeTaskQueueDepthForExecutor(state, executor_tag);
    state.deadlock_guard_passed = 0;
    return 0;
  }

  const int task_handle = state.active_group_ready_queue.front();
  auto found = state.executor_ready_queues.find(executor_tag);
  if (found == state.executor_ready_queues.end()) {
    state.last_dequeued_task_handle = 0;
    state.last_dequeued_executor_tag = executor_tag;
    state.last_executor_queue_depth = 0;
    state.race_guard_passed = 0;
    return 0;
  }

  std::deque<int> &queue = found->second;
  const auto task_position =
      std::find(queue.begin(), queue.end(), task_handle);
  if (task_position == queue.end()) {
    state.last_dequeued_task_handle = 0;
    state.last_dequeued_executor_tag = executor_tag;
    state.last_executor_queue_depth = static_cast<int>(queue.size());
    state.race_guard_passed = 0;
    return 0;
  }

  queue.erase(task_position);
  state.active_group_ready_queue.pop_front();
  ++state.scheduler_dequeue_count;
  ++state.scheduler_sequence;
  state.last_dequeued_task_handle = task_handle;
  state.last_dequeued_executor_tag = executor_tag;
  state.last_executor_queue_depth = static_cast<int>(queue.size());
  state.deadlock_guard_passed =
      RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
  state.race_guard_passed = task_handle > 0 && executor_tag >= 0 ? 1 : 0;
  return task_handle;
}

inline int CancelPendingRuntimeTaskSchedulerQueue(RuntimeTaskState &state,
                                                  int executor_tag,
                                                  int max_tasks_to_cancel) {
  auto found = state.executor_ready_queues.find(executor_tag);
  if (found == state.executor_ready_queues.end() || max_tasks_to_cancel <= 0) {
    state.last_executor_queue_depth = RuntimeTaskQueueDepthForExecutor(
        state, executor_tag);
    state.deadlock_guard_passed =
        RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
    return 0;
  }

  std::deque<int> &queue = found->second;
  int cancelled = 0;
  while (!queue.empty() && cancelled < max_tasks_to_cancel) {
    const int task_handle = queue.front();
    queue.pop_front();
    ++cancelled;
    ++state.scheduler_cancelled_count;
    ++state.scheduler_sequence;
    state.last_cancelled_task_handle = task_handle;
    state.last_cancelled_executor_tag = executor_tag;
    state.last_executor_queue_depth = static_cast<int>(queue.size());
    state.race_guard_passed = task_handle > 0 && executor_tag >= 0 ? 1 : 0;
  }
  state.deadlock_guard_passed =
      RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
  return cancelled;
}

inline int CancelPendingRuntimeTaskGroupSchedulerQueue(
    RuntimeTaskState &state,
    int executor_tag,
    int max_tasks_to_cancel) {
  auto found = state.executor_ready_queues.find(executor_tag);
  if (found == state.executor_ready_queues.end() ||
      max_tasks_to_cancel <= 0) {
    state.last_executor_queue_depth =
        RuntimeTaskQueueDepthForExecutor(state, executor_tag);
    state.deadlock_guard_passed =
        RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
    return 0;
  }

  std::deque<int> &queue = found->second;
  int cancelled = 0;
  while (!state.active_group_ready_queue.empty() &&
         cancelled < max_tasks_to_cancel) {
    const int task_handle = state.active_group_ready_queue.front();
    const auto task_position =
        std::find(queue.begin(), queue.end(), task_handle);
    if (task_position == queue.end()) {
      state.race_guard_passed = 0;
      break;
    }
    queue.erase(task_position);
    state.active_group_ready_queue.pop_front();
    ++cancelled;
    ++state.scheduler_cancelled_count;
    ++state.scheduler_sequence;
    state.last_cancelled_task_handle = task_handle;
    state.last_cancelled_executor_tag = executor_tag;
    state.last_executor_queue_depth = static_cast<int>(queue.size());
    state.race_guard_passed = task_handle > 0 && executor_tag >= 0 ? 1 : 0;
  }
  state.deadlock_guard_passed =
      RuntimeTaskSchedulerCountsAreBalanced(state) ? 1 : 0;
  return cancelled;
}

RuntimeTaskState &RuntimeTaskStateForCurrentThread();

}  // namespace objc3c::runtime
