#pragma once

#include <cstdint>
#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_probe_contract {

inline int RunTaskRuntimeProbeContract() {
  const int spawn_group = objc3_runtime_spawn_task_i32(1, 2);
  const int scope = objc3_runtime_enter_task_group_scope_i32(2);
  const int add_task = objc3_runtime_add_task_group_task_i32(2);
  objc3_runtime_task_runtime_state_snapshot after_add_snapshot{};
  const int after_add_copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&after_add_snapshot);
  const int add_second_task = objc3_runtime_add_task_group_task_i32(2);
  objc3_runtime_task_runtime_state_snapshot after_second_add_snapshot{};
  const int after_second_add_copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(
          &after_second_add_snapshot);
  const int cancelled = objc3_runtime_task_is_cancelled_i32(2);
  const int wait_next = objc3_runtime_wait_task_group_next_i32(2);
  objc3_runtime_task_runtime_state_snapshot after_wait_snapshot{};
  const int after_wait_copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&after_wait_snapshot);
  const int hop = objc3_runtime_executor_hop_i32(wait_next, 2);
  const int wait_second_next = objc3_runtime_wait_task_group_next_i32(2);
  objc3_runtime_task_runtime_state_snapshot after_second_wait_snapshot{};
  const int after_second_wait_copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(
          &after_second_wait_snapshot);
  const int cancel_all = objc3_runtime_cancel_task_group_i32(2);
  const int on_cancel = objc3_runtime_task_on_cancel_i32(2);
  const int spawn_detached = objc3_runtime_spawn_task_i32(2, 3);

  objc3_runtime_task_runtime_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&snapshot);

  std::cout << "spawn_group=" << spawn_group << "\n";
  std::cout << "scope=" << scope << "\n";
  std::cout << "add_task=" << add_task << "\n";
  std::cout << "add_second_task=" << add_second_task << "\n";
  std::cout << "cancelled=" << cancelled << "\n";
  std::cout << "wait_next=" << wait_next << "\n";
  std::cout << "wait_second_next=" << wait_second_next << "\n";
  std::cout << "hop=" << hop << "\n";
  std::cout << "cancel_all=" << cancel_all << "\n";
  std::cout << "on_cancel=" << on_cancel << "\n";
  std::cout << "spawn_detached=" << spawn_detached << "\n";
  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "after_add_copy_status=" << after_add_copy_status << "\n";
  std::cout << "after_add_last_queue_depth="
            << after_add_snapshot.last_queue_depth << "\n";
  std::cout << "after_add_scheduler_enqueue_count="
            << after_add_snapshot.scheduler_enqueue_count << "\n";
  std::cout << "after_add_scheduler_dequeue_count="
            << after_add_snapshot.scheduler_dequeue_count << "\n";
  std::cout << "after_add_last_scheduled_task_handle="
            << after_add_snapshot.last_scheduled_task_handle << "\n";
  std::cout << "after_add_last_scheduled_executor_tag="
            << after_add_snapshot.last_scheduled_executor_tag << "\n";
  std::cout << "after_add_last_executor_queue_depth="
            << after_add_snapshot.last_executor_queue_depth << "\n";
  std::cout << "after_add_max_executor_queue_depth="
            << after_add_snapshot.max_executor_queue_depth << "\n";
  std::cout << "after_add_scheduler_sequence="
            << after_add_snapshot.scheduler_sequence << "\n";
  std::cout << "after_add_deadlock_guard_passed="
            << after_add_snapshot.deadlock_guard_passed << "\n";
  std::cout << "after_add_race_guard_passed="
            << after_add_snapshot.race_guard_passed << "\n";
  std::cout << "after_second_add_copy_status="
            << after_second_add_copy_status << "\n";
  std::cout << "after_second_add_last_queue_depth="
            << after_second_add_snapshot.last_queue_depth << "\n";
  std::cout << "after_second_add_scheduler_enqueue_count="
            << after_second_add_snapshot.scheduler_enqueue_count << "\n";
  std::cout << "after_second_add_scheduler_dequeue_count="
            << after_second_add_snapshot.scheduler_dequeue_count << "\n";
  std::cout << "after_second_add_last_scheduled_task_handle="
            << after_second_add_snapshot.last_scheduled_task_handle << "\n";
  std::cout << "after_second_add_last_scheduled_executor_tag="
            << after_second_add_snapshot.last_scheduled_executor_tag << "\n";
  std::cout << "after_second_add_last_executor_queue_depth="
            << after_second_add_snapshot.last_executor_queue_depth << "\n";
  std::cout << "after_second_add_max_executor_queue_depth="
            << after_second_add_snapshot.max_executor_queue_depth << "\n";
  std::cout << "after_second_add_scheduler_sequence="
            << after_second_add_snapshot.scheduler_sequence << "\n";
  std::cout << "after_second_add_deadlock_guard_passed="
            << after_second_add_snapshot.deadlock_guard_passed << "\n";
  std::cout << "after_second_add_race_guard_passed="
            << after_second_add_snapshot.race_guard_passed << "\n";
  std::cout << "after_wait_copy_status=" << after_wait_copy_status << "\n";
  std::cout << "after_wait_last_queue_depth="
            << after_wait_snapshot.last_queue_depth << "\n";
  std::cout << "after_wait_last_queue_drain_result="
            << after_wait_snapshot.last_queue_drain_result << "\n";
  std::cout << "after_wait_scheduler_enqueue_count="
            << after_wait_snapshot.scheduler_enqueue_count << "\n";
  std::cout << "after_wait_scheduler_dequeue_count="
            << after_wait_snapshot.scheduler_dequeue_count << "\n";
  std::cout << "after_wait_last_scheduled_task_handle="
            << after_wait_snapshot.last_scheduled_task_handle << "\n";
  std::cout << "after_wait_last_scheduled_executor_tag="
            << after_wait_snapshot.last_scheduled_executor_tag << "\n";
  std::cout << "after_wait_last_dequeued_task_handle="
            << after_wait_snapshot.last_dequeued_task_handle << "\n";
  std::cout << "after_wait_last_dequeued_executor_tag="
            << after_wait_snapshot.last_dequeued_executor_tag << "\n";
  std::cout << "after_wait_last_executor_queue_depth="
            << after_wait_snapshot.last_executor_queue_depth << "\n";
  std::cout << "after_wait_max_executor_queue_depth="
            << after_wait_snapshot.max_executor_queue_depth << "\n";
  std::cout << "after_wait_scheduler_sequence="
            << after_wait_snapshot.scheduler_sequence << "\n";
  std::cout << "after_wait_deadlock_guard_passed="
            << after_wait_snapshot.deadlock_guard_passed << "\n";
  std::cout << "after_wait_race_guard_passed="
            << after_wait_snapshot.race_guard_passed << "\n";
  std::cout << "after_second_wait_copy_status="
            << after_second_wait_copy_status << "\n";
  std::cout << "after_second_wait_last_queue_depth="
            << after_second_wait_snapshot.last_queue_depth << "\n";
  std::cout << "after_second_wait_last_queue_drain_result="
            << after_second_wait_snapshot.last_queue_drain_result << "\n";
  std::cout << "after_second_wait_scheduler_enqueue_count="
            << after_second_wait_snapshot.scheduler_enqueue_count << "\n";
  std::cout << "after_second_wait_scheduler_dequeue_count="
            << after_second_wait_snapshot.scheduler_dequeue_count << "\n";
  std::cout << "after_second_wait_last_scheduled_task_handle="
            << after_second_wait_snapshot.last_scheduled_task_handle << "\n";
  std::cout << "after_second_wait_last_scheduled_executor_tag="
            << after_second_wait_snapshot.last_scheduled_executor_tag << "\n";
  std::cout << "after_second_wait_last_dequeued_task_handle="
            << after_second_wait_snapshot.last_dequeued_task_handle << "\n";
  std::cout << "after_second_wait_last_dequeued_executor_tag="
            << after_second_wait_snapshot.last_dequeued_executor_tag << "\n";
  std::cout << "after_second_wait_last_executor_queue_depth="
            << after_second_wait_snapshot.last_executor_queue_depth << "\n";
  std::cout << "after_second_wait_max_executor_queue_depth="
            << after_second_wait_snapshot.max_executor_queue_depth << "\n";
  std::cout << "after_second_wait_scheduler_sequence="
            << after_second_wait_snapshot.scheduler_sequence << "\n";
  std::cout << "after_second_wait_deadlock_guard_passed="
            << after_second_wait_snapshot.deadlock_guard_passed << "\n";
  std::cout << "after_second_wait_race_guard_passed="
            << after_second_wait_snapshot.race_guard_passed << "\n";
  std::cout << "spawn_call_count=" << snapshot.spawn_call_count << "\n";
  std::cout << "scope_call_count=" << snapshot.scope_call_count << "\n";
  std::cout << "add_task_call_count=" << snapshot.add_task_call_count << "\n";
  std::cout << "wait_next_call_count=" << snapshot.wait_next_call_count << "\n";
  std::cout << "cancel_all_call_count=" << snapshot.cancel_all_call_count
            << "\n";
  std::cout << "cancellation_poll_call_count="
            << snapshot.cancellation_poll_call_count << "\n";
  std::cout << "on_cancel_call_count=" << snapshot.on_cancel_call_count
            << "\n";
  std::cout << "executor_hop_call_count=" << snapshot.executor_hop_call_count
            << "\n";
  std::cout << "last_spawn_kind=" << snapshot.last_spawn_kind << "\n";
  std::cout << "last_spawn_executor_tag=" << snapshot.last_spawn_executor_tag
            << "\n";
  std::cout << "last_wait_next_result=" << snapshot.last_wait_next_result
            << "\n";
  std::cout << "last_executor_hop_executor_tag="
            << snapshot.last_executor_hop_executor_tag << "\n";
  std::cout << "last_executor_hop_value="
            << snapshot.last_executor_hop_value << "\n";
  std::cout << "last_failure_reason=" << snapshot.last_failure_reason << "\n";
  std::cout << "lifecycle_state=" << snapshot.lifecycle_state << "\n";
  std::cout << "selected_executor_tag=" << snapshot.selected_executor_tag
            << "\n";
  std::cout << "active_group_executor_tag="
            << snapshot.active_group_executor_tag << "\n";
  std::cout << "active_group_task_count="
            << snapshot.active_group_task_count << "\n";
  std::cout << "pending_group_task_count="
            << snapshot.pending_group_task_count << "\n";
  std::cout << "completed_group_task_count="
            << snapshot.completed_group_task_count << "\n";
  std::cout << "cancelled_group_task_count="
            << snapshot.cancelled_group_task_count << "\n";
  std::cout << "group_cancelled=" << snapshot.group_cancelled << "\n";
  std::cout << "cancellation_generation="
            << snapshot.cancellation_generation << "\n";
  std::cout << "observed_cancellation_generation="
            << snapshot.observed_cancellation_generation << "\n";
  std::cout << "last_queue_depth=" << snapshot.last_queue_depth << "\n";
  std::cout << "last_queue_drain_result="
            << snapshot.last_queue_drain_result << "\n";
  std::cout << "scheduler_enqueue_count="
            << snapshot.scheduler_enqueue_count << "\n";
  std::cout << "scheduler_dequeue_count="
            << snapshot.scheduler_dequeue_count << "\n";
  std::cout << "scheduler_cancelled_count="
            << snapshot.scheduler_cancelled_count << "\n";
  std::cout << "last_scheduled_task_handle="
            << snapshot.last_scheduled_task_handle << "\n";
  std::cout << "last_scheduled_executor_tag="
            << snapshot.last_scheduled_executor_tag << "\n";
  std::cout << "last_dequeued_task_handle="
            << snapshot.last_dequeued_task_handle << "\n";
  std::cout << "last_dequeued_executor_tag="
            << snapshot.last_dequeued_executor_tag << "\n";
  std::cout << "last_cancelled_task_handle="
            << snapshot.last_cancelled_task_handle << "\n";
  std::cout << "last_cancelled_executor_tag="
            << snapshot.last_cancelled_executor_tag << "\n";
  std::cout << "last_executor_queue_depth="
            << snapshot.last_executor_queue_depth << "\n";
  std::cout << "max_executor_queue_depth="
            << snapshot.max_executor_queue_depth << "\n";
  std::cout << "scheduler_sequence=" << snapshot.scheduler_sequence << "\n";
  std::cout << "deadlock_guard_passed="
            << snapshot.deadlock_guard_passed << "\n";
  std::cout << "race_guard_passed=" << snapshot.race_guard_passed << "\n";

  objc3_runtime_reset_for_testing();
  const int cancel_drain_scope = objc3_runtime_enter_task_group_scope_i32(6);
  const int cancel_drain_add_task =
      objc3_runtime_add_task_group_task_i32(6);
  const int cancel_drain_add_second_task =
      objc3_runtime_add_task_group_task_i32(6);
  const int cancel_drain_cancel_all =
      objc3_runtime_cancel_task_group_i32(6);
  objc3_runtime_task_runtime_state_snapshot cancel_drain_snapshot{};
  const int cancel_drain_copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(
          &cancel_drain_snapshot);

  std::cout << "cancel_drain_scope=" << cancel_drain_scope << "\n";
  std::cout << "cancel_drain_add_task=" << cancel_drain_add_task << "\n";
  std::cout << "cancel_drain_add_second_task="
            << cancel_drain_add_second_task << "\n";
  std::cout << "cancel_drain_cancel_all=" << cancel_drain_cancel_all << "\n";
  std::cout << "cancel_drain_copy_task_status="
            << cancel_drain_copy_status << "\n";
  std::cout << "cancel_drain_scope_call_count="
            << cancel_drain_snapshot.scope_call_count << "\n";
  std::cout << "cancel_drain_add_task_call_count="
            << cancel_drain_snapshot.add_task_call_count << "\n";
  std::cout << "cancel_drain_cancel_all_call_count="
            << cancel_drain_snapshot.cancel_all_call_count << "\n";
  std::cout << "cancel_drain_last_cancel_all_result="
            << cancel_drain_snapshot.last_cancel_all_result << "\n";
  std::cout << "cancel_drain_last_failure_reason="
            << cancel_drain_snapshot.last_failure_reason << "\n";
  std::cout << "cancel_drain_lifecycle_state="
            << cancel_drain_snapshot.lifecycle_state << "\n";
  std::cout << "cancel_drain_selected_executor_tag="
            << cancel_drain_snapshot.selected_executor_tag << "\n";
  std::cout << "cancel_drain_active_group_executor_tag="
            << cancel_drain_snapshot.active_group_executor_tag << "\n";
  std::cout << "cancel_drain_active_group_task_count="
            << cancel_drain_snapshot.active_group_task_count << "\n";
  std::cout << "cancel_drain_pending_group_task_count="
            << cancel_drain_snapshot.pending_group_task_count << "\n";
  std::cout << "cancel_drain_completed_group_task_count="
            << cancel_drain_snapshot.completed_group_task_count << "\n";
  std::cout << "cancel_drain_cancelled_group_task_count="
            << cancel_drain_snapshot.cancelled_group_task_count << "\n";
  std::cout << "cancel_drain_group_cancelled="
            << cancel_drain_snapshot.group_cancelled << "\n";
  std::cout << "cancel_drain_cancellation_generation="
            << cancel_drain_snapshot.cancellation_generation << "\n";
  std::cout << "cancel_drain_last_queue_depth="
            << cancel_drain_snapshot.last_queue_depth << "\n";
  std::cout << "cancel_drain_last_queue_drain_result="
            << cancel_drain_snapshot.last_queue_drain_result << "\n";
  std::cout << "cancel_drain_scheduler_enqueue_count="
            << cancel_drain_snapshot.scheduler_enqueue_count << "\n";
  std::cout << "cancel_drain_scheduler_dequeue_count="
            << cancel_drain_snapshot.scheduler_dequeue_count << "\n";
  std::cout << "cancel_drain_scheduler_cancelled_count="
            << cancel_drain_snapshot.scheduler_cancelled_count << "\n";
  std::cout << "cancel_drain_last_scheduled_task_handle="
            << cancel_drain_snapshot.last_scheduled_task_handle << "\n";
  std::cout << "cancel_drain_last_scheduled_executor_tag="
            << cancel_drain_snapshot.last_scheduled_executor_tag << "\n";
  std::cout << "cancel_drain_last_cancelled_task_handle="
            << cancel_drain_snapshot.last_cancelled_task_handle << "\n";
  std::cout << "cancel_drain_last_cancelled_executor_tag="
            << cancel_drain_snapshot.last_cancelled_executor_tag << "\n";
  std::cout << "cancel_drain_last_executor_queue_depth="
            << cancel_drain_snapshot.last_executor_queue_depth << "\n";
  std::cout << "cancel_drain_max_executor_queue_depth="
            << cancel_drain_snapshot.max_executor_queue_depth << "\n";
  std::cout << "cancel_drain_scheduler_sequence="
            << cancel_drain_snapshot.scheduler_sequence << "\n";
  std::cout << "cancel_drain_deadlock_guard_passed="
            << cancel_drain_snapshot.deadlock_guard_passed << "\n";
  std::cout << "cancel_drain_race_guard_passed="
            << cancel_drain_snapshot.race_guard_passed << "\n";
  std::cout << "cancel_drain_replay_equal=1\n";

  return (copy_status == 0 && after_add_copy_status == 0 &&
          after_second_add_copy_status == 0 && after_wait_copy_status == 0 &&
          after_second_wait_copy_status == 0 && spawn_group == 111 &&
          scope == 1 && add_task == 1 && add_second_task == 1 &&
          cancelled == 0 && wait_next == 23 && wait_second_next == 24 &&
          hop == 23 && cancel_all == 31 &&
          after_add_snapshot.last_queue_depth == 1 &&
          after_add_snapshot.scheduler_enqueue_count == 1 &&
          after_add_snapshot.scheduler_dequeue_count == 0 &&
          after_add_snapshot.last_scheduled_task_handle == 23 &&
          after_add_snapshot.last_scheduled_executor_tag == 2 &&
          after_add_snapshot.last_executor_queue_depth == 1 &&
          after_add_snapshot.max_executor_queue_depth == 1 &&
          after_add_snapshot.scheduler_sequence == 1 &&
          after_add_snapshot.deadlock_guard_passed == 1 &&
          after_add_snapshot.race_guard_passed == 1 &&
          after_second_add_snapshot.last_queue_depth == 2 &&
          after_second_add_snapshot.scheduler_enqueue_count == 2 &&
          after_second_add_snapshot.scheduler_dequeue_count == 0 &&
          after_second_add_snapshot.last_scheduled_task_handle == 24 &&
          after_second_add_snapshot.last_scheduled_executor_tag == 2 &&
          after_second_add_snapshot.last_executor_queue_depth == 2 &&
          after_second_add_snapshot.max_executor_queue_depth == 2 &&
          after_second_add_snapshot.scheduler_sequence == 2 &&
          after_second_add_snapshot.deadlock_guard_passed == 1 &&
          after_second_add_snapshot.race_guard_passed == 1 &&
          after_wait_snapshot.last_queue_depth == 1 &&
          after_wait_snapshot.last_queue_drain_result == 23 &&
          after_wait_snapshot.scheduler_enqueue_count == 2 &&
          after_wait_snapshot.scheduler_dequeue_count == 1 &&
          after_wait_snapshot.last_scheduled_task_handle == 24 &&
          after_wait_snapshot.last_scheduled_executor_tag == 2 &&
          after_wait_snapshot.last_dequeued_task_handle == 23 &&
          after_wait_snapshot.last_dequeued_executor_tag == 2 &&
          after_wait_snapshot.last_executor_queue_depth == 1 &&
          after_wait_snapshot.max_executor_queue_depth == 2 &&
          after_wait_snapshot.scheduler_sequence == 3 &&
          after_wait_snapshot.deadlock_guard_passed == 1 &&
          after_wait_snapshot.race_guard_passed == 1 &&
          after_second_wait_snapshot.last_queue_depth == 0 &&
          after_second_wait_snapshot.last_queue_drain_result == 24 &&
          after_second_wait_snapshot.scheduler_enqueue_count == 2 &&
          after_second_wait_snapshot.scheduler_dequeue_count == 2 &&
          after_second_wait_snapshot.last_scheduled_task_handle == 24 &&
          after_second_wait_snapshot.last_scheduled_executor_tag == 2 &&
          after_second_wait_snapshot.last_dequeued_task_handle == 24 &&
          after_second_wait_snapshot.last_dequeued_executor_tag == 2 &&
          after_second_wait_snapshot.last_executor_queue_depth == 0 &&
          after_second_wait_snapshot.max_executor_queue_depth == 2 &&
          after_second_wait_snapshot.scheduler_sequence == 4 &&
          after_second_wait_snapshot.deadlock_guard_passed == 1 &&
          after_second_wait_snapshot.race_guard_passed == 1 &&
          on_cancel == 41 && spawn_detached == 121 &&
          snapshot.spawn_call_count == 2 && snapshot.scope_call_count == 1 &&
          snapshot.add_task_call_count == 2 &&
          snapshot.wait_next_call_count == 2 &&
          snapshot.cancel_all_call_count == 1 &&
          snapshot.cancellation_poll_call_count == 1 &&
          snapshot.on_cancel_call_count == 1 &&
          snapshot.executor_hop_call_count == 1 &&
          snapshot.last_spawn_kind == 2 &&
          snapshot.last_spawn_executor_tag == 3 &&
          snapshot.last_wait_next_result == 24 &&
          snapshot.last_executor_hop_executor_tag == 2 &&
          snapshot.last_executor_hop_value == 23 &&
          snapshot.last_failure_reason == OBJC3_RUNTIME_TASK_FAILURE_NONE &&
          snapshot.lifecycle_state ==
              OBJC3_RUNTIME_TASK_LIFECYCLE_TASK_SPAWNED &&
          snapshot.selected_executor_tag == 3 &&
          snapshot.active_group_executor_tag == 2 &&
          snapshot.active_group_task_count == 2 &&
          snapshot.pending_group_task_count == 0 &&
          snapshot.completed_group_task_count == 2 &&
          snapshot.cancelled_group_task_count == 0 &&
          snapshot.group_cancelled == 1 &&
          snapshot.cancellation_generation == 1 &&
          snapshot.observed_cancellation_generation == 1 &&
          snapshot.last_queue_depth == 0 &&
          snapshot.last_queue_drain_result == 24 &&
          snapshot.scheduler_enqueue_count == 3 &&
          snapshot.scheduler_dequeue_count == 2 &&
          snapshot.scheduler_cancelled_count == 0 &&
          snapshot.last_scheduled_task_handle == 121 &&
          snapshot.last_scheduled_executor_tag == 3 &&
          snapshot.last_dequeued_task_handle == 24 &&
          snapshot.last_dequeued_executor_tag == 2 &&
          snapshot.last_cancelled_task_handle == 0 &&
          snapshot.last_cancelled_executor_tag == 0 &&
          snapshot.last_executor_queue_depth == 1 &&
          snapshot.max_executor_queue_depth == 2 &&
          snapshot.scheduler_sequence == 5 &&
          snapshot.deadlock_guard_passed == 1 &&
          snapshot.race_guard_passed == 1 &&
          cancel_drain_scope == 1 && cancel_drain_add_task == 1 &&
          cancel_drain_add_second_task == 1 &&
          cancel_drain_cancel_all == 31 &&
          cancel_drain_copy_status == 0 &&
          cancel_drain_snapshot.scope_call_count == 1 &&
          cancel_drain_snapshot.add_task_call_count == 2 &&
          cancel_drain_snapshot.cancel_all_call_count == 1 &&
          cancel_drain_snapshot.last_cancel_all_result == 31 &&
          cancel_drain_snapshot.last_failure_reason ==
              OBJC3_RUNTIME_TASK_FAILURE_NONE &&
          cancel_drain_snapshot.lifecycle_state ==
              OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_CANCELLED &&
          cancel_drain_snapshot.selected_executor_tag == 6 &&
          cancel_drain_snapshot.active_group_executor_tag == 6 &&
          cancel_drain_snapshot.active_group_task_count == 2 &&
          cancel_drain_snapshot.pending_group_task_count == 0 &&
          cancel_drain_snapshot.completed_group_task_count == 0 &&
          cancel_drain_snapshot.cancelled_group_task_count == 2 &&
          cancel_drain_snapshot.group_cancelled == 1 &&
          cancel_drain_snapshot.cancellation_generation == 1 &&
          cancel_drain_snapshot.last_queue_depth == 0 &&
          cancel_drain_snapshot.last_queue_drain_result == 28 &&
          cancel_drain_snapshot.scheduler_enqueue_count == 2 &&
          cancel_drain_snapshot.scheduler_dequeue_count == 0 &&
          cancel_drain_snapshot.scheduler_cancelled_count == 2 &&
          cancel_drain_snapshot.last_scheduled_task_handle == 28 &&
          cancel_drain_snapshot.last_scheduled_executor_tag == 6 &&
          cancel_drain_snapshot.last_cancelled_task_handle == 28 &&
          cancel_drain_snapshot.last_cancelled_executor_tag == 6 &&
          cancel_drain_snapshot.last_executor_queue_depth == 0 &&
          cancel_drain_snapshot.max_executor_queue_depth == 2 &&
          cancel_drain_snapshot.scheduler_sequence == 4 &&
          cancel_drain_snapshot.deadlock_guard_passed == 1 &&
          cancel_drain_snapshot.race_guard_passed == 1)
             ? 0
             : 1;
}

} // namespace task_runtime_probe_contract
} // namespace tooling
} // namespace objc3c
