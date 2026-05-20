#pragma once

#include <iostream>
#include <ostream>

#include "probe_state.h"
#include "runtime_assertion_helpers.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline void WriteProbeReport(const ProbeRun &run, std::ostream &out) {
  const PassResult &pass1 = run.pass1;

  out << "pass1_copy_task_status=" << pass1.copy_task_status << "\n";
  out << "pass1_after_add_copy_status="
      << pass1.after_add_copy_status << "\n";
  out << "pass1_after_second_add_copy_status="
      << pass1.after_second_add_copy_status << "\n";
  out << "pass1_after_wait_copy_status="
      << pass1.after_wait_copy_status << "\n";
  out << "pass1_after_second_wait_copy_status="
      << pass1.after_second_wait_copy_status << "\n";
  out << "pass1_copy_memory_status=" << pass1.copy_memory_status << "\n";
  out << "pass1_copy_arc_status=" << pass1.copy_arc_status << "\n";
  out << "pass1_spawn_group=" << pass1.spawn_group << "\n";
  out << "pass1_scope=" << pass1.scope << "\n";
  out << "pass1_add_task=" << pass1.add_task << "\n";
  out << "pass1_add_second_task=" << pass1.add_second_task << "\n";
  out << "pass1_cancelled=" << pass1.cancelled << "\n";
  out << "pass1_wait_next=" << pass1.wait_next << "\n";
  out << "pass1_wait_second_next=" << pass1.wait_second_next << "\n";
  out << "pass1_hop=" << pass1.hop << "\n";
  out << "pass1_cancel_all=" << pass1.cancel_all << "\n";
  out << "pass1_on_cancel=" << pass1.on_cancel << "\n";
  out << "pass1_spawn_detached=" << pass1.spawn_detached << "\n";
  out << "pass1_spawn_call_count=" << pass1.task.spawn_call_count << "\n";
  out << "pass1_scope_call_count=" << pass1.task.scope_call_count << "\n";
  out << "pass1_add_task_call_count="
      << pass1.task.add_task_call_count << "\n";
  out << "pass1_wait_next_call_count="
      << pass1.task.wait_next_call_count << "\n";
  out << "pass1_cancel_all_call_count="
      << pass1.task.cancel_all_call_count << "\n";
  out << "pass1_cancellation_poll_call_count="
      << pass1.task.cancellation_poll_call_count << "\n";
  out << "pass1_on_cancel_call_count="
      << pass1.task.on_cancel_call_count << "\n";
  out << "pass1_executor_hop_call_count="
      << pass1.task.executor_hop_call_count << "\n";
  out << "pass1_last_spawn_kind=" << pass1.task.last_spawn_kind << "\n";
  out << "pass1_last_spawn_executor_tag="
      << pass1.task.last_spawn_executor_tag << "\n";
  out << "pass1_last_wait_next_result="
      << pass1.task.last_wait_next_result << "\n";
  out << "pass1_last_executor_hop_executor_tag="
      << pass1.task.last_executor_hop_executor_tag << "\n";
  out << "pass1_last_executor_hop_value="
      << pass1.task.last_executor_hop_value << "\n";
  out << "pass1_after_add_last_queue_depth="
      << pass1.after_add_task.last_queue_depth << "\n";
  out << "pass1_after_add_scheduler_enqueue_count="
      << pass1.after_add_task.scheduler_enqueue_count << "\n";
  out << "pass1_after_add_scheduler_dequeue_count="
      << pass1.after_add_task.scheduler_dequeue_count << "\n";
  out << "pass1_after_add_last_scheduled_task_handle="
      << pass1.after_add_task.last_scheduled_task_handle << "\n";
  out << "pass1_after_add_last_scheduled_executor_tag="
      << pass1.after_add_task.last_scheduled_executor_tag << "\n";
  out << "pass1_after_add_last_executor_queue_depth="
      << pass1.after_add_task.last_executor_queue_depth << "\n";
  out << "pass1_after_add_max_executor_queue_depth="
      << pass1.after_add_task.max_executor_queue_depth << "\n";
  out << "pass1_after_add_scheduler_sequence="
      << pass1.after_add_task.scheduler_sequence << "\n";
  out << "pass1_after_add_deadlock_guard_passed="
      << pass1.after_add_task.deadlock_guard_passed << "\n";
  out << "pass1_after_add_race_guard_passed="
      << pass1.after_add_task.race_guard_passed << "\n";
  out << "pass1_after_second_add_last_queue_depth="
      << pass1.after_second_add_task.last_queue_depth << "\n";
  out << "pass1_after_second_add_scheduler_enqueue_count="
      << pass1.after_second_add_task.scheduler_enqueue_count << "\n";
  out << "pass1_after_second_add_scheduler_dequeue_count="
      << pass1.after_second_add_task.scheduler_dequeue_count << "\n";
  out << "pass1_after_second_add_last_scheduled_task_handle="
      << pass1.after_second_add_task.last_scheduled_task_handle << "\n";
  out << "pass1_after_second_add_last_scheduled_executor_tag="
      << pass1.after_second_add_task.last_scheduled_executor_tag << "\n";
  out << "pass1_after_second_add_last_executor_queue_depth="
      << pass1.after_second_add_task.last_executor_queue_depth << "\n";
  out << "pass1_after_second_add_max_executor_queue_depth="
      << pass1.after_second_add_task.max_executor_queue_depth << "\n";
  out << "pass1_after_second_add_scheduler_sequence="
      << pass1.after_second_add_task.scheduler_sequence << "\n";
  out << "pass1_after_second_add_deadlock_guard_passed="
      << pass1.after_second_add_task.deadlock_guard_passed << "\n";
  out << "pass1_after_second_add_race_guard_passed="
      << pass1.after_second_add_task.race_guard_passed << "\n";
  out << "pass1_after_wait_last_queue_depth="
      << pass1.after_wait_next.last_queue_depth << "\n";
  out << "pass1_after_wait_last_queue_drain_result="
      << pass1.after_wait_next.last_queue_drain_result << "\n";
  out << "pass1_after_wait_scheduler_enqueue_count="
      << pass1.after_wait_next.scheduler_enqueue_count << "\n";
  out << "pass1_after_wait_scheduler_dequeue_count="
      << pass1.after_wait_next.scheduler_dequeue_count << "\n";
  out << "pass1_after_wait_last_scheduled_task_handle="
      << pass1.after_wait_next.last_scheduled_task_handle << "\n";
  out << "pass1_after_wait_last_dequeued_task_handle="
      << pass1.after_wait_next.last_dequeued_task_handle << "\n";
  out << "pass1_after_wait_last_scheduled_executor_tag="
      << pass1.after_wait_next.last_scheduled_executor_tag << "\n";
  out << "pass1_after_wait_last_dequeued_executor_tag="
      << pass1.after_wait_next.last_dequeued_executor_tag << "\n";
  out << "pass1_after_wait_last_executor_queue_depth="
      << pass1.after_wait_next.last_executor_queue_depth << "\n";
  out << "pass1_after_wait_max_executor_queue_depth="
      << pass1.after_wait_next.max_executor_queue_depth << "\n";
  out << "pass1_after_wait_scheduler_sequence="
      << pass1.after_wait_next.scheduler_sequence << "\n";
  out << "pass1_after_wait_deadlock_guard_passed="
      << pass1.after_wait_next.deadlock_guard_passed << "\n";
  out << "pass1_after_wait_race_guard_passed="
      << pass1.after_wait_next.race_guard_passed << "\n";
  out << "pass1_after_second_wait_last_queue_depth="
      << pass1.after_second_wait_next.last_queue_depth << "\n";
  out << "pass1_after_second_wait_last_queue_drain_result="
      << pass1.after_second_wait_next.last_queue_drain_result << "\n";
  out << "pass1_after_second_wait_scheduler_enqueue_count="
      << pass1.after_second_wait_next.scheduler_enqueue_count << "\n";
  out << "pass1_after_second_wait_scheduler_dequeue_count="
      << pass1.after_second_wait_next.scheduler_dequeue_count << "\n";
  out << "pass1_after_second_wait_last_scheduled_task_handle="
      << pass1.after_second_wait_next.last_scheduled_task_handle << "\n";
  out << "pass1_after_second_wait_last_scheduled_executor_tag="
      << pass1.after_second_wait_next.last_scheduled_executor_tag << "\n";
  out << "pass1_after_second_wait_last_dequeued_task_handle="
      << pass1.after_second_wait_next.last_dequeued_task_handle << "\n";
  out << "pass1_after_second_wait_last_dequeued_executor_tag="
      << pass1.after_second_wait_next.last_dequeued_executor_tag << "\n";
  out << "pass1_after_second_wait_last_executor_queue_depth="
      << pass1.after_second_wait_next.last_executor_queue_depth << "\n";
  out << "pass1_after_second_wait_max_executor_queue_depth="
      << pass1.after_second_wait_next.max_executor_queue_depth << "\n";
  out << "pass1_after_second_wait_scheduler_sequence="
      << pass1.after_second_wait_next.scheduler_sequence << "\n";
  out << "pass1_after_second_wait_deadlock_guard_passed="
      << pass1.after_second_wait_next.deadlock_guard_passed << "\n";
  out << "pass1_after_second_wait_race_guard_passed="
      << pass1.after_second_wait_next.race_guard_passed << "\n";
  out << "pass1_last_failure_reason="
      << pass1.task.last_failure_reason << "\n";
  out << "pass1_lifecycle_state=" << pass1.task.lifecycle_state << "\n";
  out << "pass1_selected_executor_tag="
      << pass1.task.selected_executor_tag << "\n";
  out << "pass1_active_group_executor_tag="
      << pass1.task.active_group_executor_tag << "\n";
  out << "pass1_active_group_task_count="
      << pass1.task.active_group_task_count << "\n";
  out << "pass1_pending_group_task_count="
      << pass1.task.pending_group_task_count << "\n";
  out << "pass1_completed_group_task_count="
      << pass1.task.completed_group_task_count << "\n";
  out << "pass1_cancelled_group_task_count="
      << pass1.task.cancelled_group_task_count << "\n";
  out << "pass1_group_cancelled=" << pass1.task.group_cancelled << "\n";
  out << "pass1_cancellation_generation="
      << pass1.task.cancellation_generation << "\n";
  out << "pass1_observed_cancellation_generation="
      << pass1.task.observed_cancellation_generation << "\n";
  out << "pass1_last_queue_depth="
      << pass1.task.last_queue_depth << "\n";
  out << "pass1_last_queue_drain_result="
      << pass1.task.last_queue_drain_result << "\n";
  out << "pass1_scheduler_enqueue_count="
      << pass1.task.scheduler_enqueue_count << "\n";
  out << "pass1_scheduler_dequeue_count="
      << pass1.task.scheduler_dequeue_count << "\n";
  out << "pass1_scheduler_cancelled_count="
      << pass1.task.scheduler_cancelled_count << "\n";
  out << "pass1_last_scheduled_task_handle="
      << pass1.task.last_scheduled_task_handle << "\n";
  out << "pass1_last_scheduled_executor_tag="
      << pass1.task.last_scheduled_executor_tag << "\n";
  out << "pass1_last_dequeued_task_handle="
      << pass1.task.last_dequeued_task_handle << "\n";
  out << "pass1_last_dequeued_executor_tag="
      << pass1.task.last_dequeued_executor_tag << "\n";
  out << "pass1_last_cancelled_task_handle="
      << pass1.task.last_cancelled_task_handle << "\n";
  out << "pass1_last_cancelled_executor_tag="
      << pass1.task.last_cancelled_executor_tag << "\n";
  out << "pass1_last_executor_queue_depth="
      << pass1.task.last_executor_queue_depth << "\n";
  out << "pass1_max_executor_queue_depth="
      << pass1.task.max_executor_queue_depth << "\n";
  out << "pass1_scheduler_sequence="
      << pass1.task.scheduler_sequence << "\n";
  out << "pass1_deadlock_guard_passed="
      << pass1.task.deadlock_guard_passed << "\n";
  out << "pass1_race_guard_passed="
      << pass1.task.race_guard_passed << "\n";
  out << "pass1_autoreleasepool_depth="
      << pass1.memory.autoreleasepool_depth << "\n";
  out << "pass1_autoreleasepool_max_depth="
      << pass1.memory.autoreleasepool_max_depth << "\n";
  out << "pass1_autoreleasepool_push_count="
      << pass1.arc.autoreleasepool_push_count << "\n";
  out << "pass1_autoreleasepool_pop_count="
      << pass1.arc.autoreleasepool_pop_count << "\n";
  out << "invalid1_invalid_spawn_kind="
      << run.invalid1.invalid_spawn_kind << "\n";
  out << "invalid1_invalid_spawn_executor="
      << run.invalid1.invalid_spawn_executor << "\n";
  out << "invalid1_missing_group_wait="
      << run.invalid1.missing_group_wait << "\n";
  out << "invalid1_missing_group_cancel="
      << run.invalid1.missing_group_cancel << "\n";
  out << "invalid1_executor_mismatch_add="
      << run.invalid1.executor_mismatch_add << "\n";
  out << "invalid1_last_failure_reason="
      << run.invalid1.task.last_failure_reason << "\n";
  out << "invalid1_last_wait_next_result="
      << run.invalid1.task.last_wait_next_result << "\n";
  out << "invalid1_last_cancel_all_result="
      << run.invalid1.task.last_cancel_all_result << "\n";
  out << "invalid1_active_group_executor_tag="
      << run.invalid1.task.active_group_executor_tag << "\n";
  out << "invalid1_scheduler_enqueue_count="
      << run.invalid1.task.scheduler_enqueue_count << "\n";
  out << "invalid1_scheduler_cancelled_count="
      << run.invalid1.task.scheduler_cancelled_count << "\n";
  out << "invalid1_deadlock_guard_passed="
      << run.invalid1.task.deadlock_guard_passed << "\n";
  out << "invalid1_race_guard_passed="
      << run.invalid1.task.race_guard_passed << "\n";
  out << "cancel_drain_scope=" << run.cancel_drain1.scope << "\n";
  out << "cancel_drain_add_task=" << run.cancel_drain1.add_task << "\n";
  out << "cancel_drain_add_second_task="
      << run.cancel_drain1.add_second_task << "\n";
  out << "cancel_drain_cancel_all=" << run.cancel_drain1.cancel_all << "\n";
  out << "cancel_drain_copy_task_status="
      << run.cancel_drain1.copy_task_status << "\n";
  out << "cancel_drain_scope_call_count="
      << run.cancel_drain1.task.scope_call_count << "\n";
  out << "cancel_drain_add_task_call_count="
      << run.cancel_drain1.task.add_task_call_count << "\n";
  out << "cancel_drain_cancel_all_call_count="
      << run.cancel_drain1.task.cancel_all_call_count << "\n";
  out << "cancel_drain_last_cancel_all_result="
      << run.cancel_drain1.task.last_cancel_all_result << "\n";
  out << "cancel_drain_last_failure_reason="
      << run.cancel_drain1.task.last_failure_reason << "\n";
  out << "cancel_drain_lifecycle_state="
      << run.cancel_drain1.task.lifecycle_state << "\n";
  out << "cancel_drain_selected_executor_tag="
      << run.cancel_drain1.task.selected_executor_tag << "\n";
  out << "cancel_drain_active_group_executor_tag="
      << run.cancel_drain1.task.active_group_executor_tag << "\n";
  out << "cancel_drain_active_group_task_count="
      << run.cancel_drain1.task.active_group_task_count << "\n";
  out << "cancel_drain_pending_group_task_count="
      << run.cancel_drain1.task.pending_group_task_count << "\n";
  out << "cancel_drain_completed_group_task_count="
      << run.cancel_drain1.task.completed_group_task_count << "\n";
  out << "cancel_drain_cancelled_group_task_count="
      << run.cancel_drain1.task.cancelled_group_task_count << "\n";
  out << "cancel_drain_group_cancelled="
      << run.cancel_drain1.task.group_cancelled << "\n";
  out << "cancel_drain_cancellation_generation="
      << run.cancel_drain1.task.cancellation_generation << "\n";
  out << "cancel_drain_last_queue_depth="
      << run.cancel_drain1.task.last_queue_depth << "\n";
  out << "cancel_drain_last_queue_drain_result="
      << run.cancel_drain1.task.last_queue_drain_result << "\n";
  out << "cancel_drain_scheduler_enqueue_count="
      << run.cancel_drain1.task.scheduler_enqueue_count << "\n";
  out << "cancel_drain_scheduler_dequeue_count="
      << run.cancel_drain1.task.scheduler_dequeue_count << "\n";
  out << "cancel_drain_scheduler_cancelled_count="
      << run.cancel_drain1.task.scheduler_cancelled_count << "\n";
  out << "cancel_drain_last_scheduled_task_handle="
      << run.cancel_drain1.task.last_scheduled_task_handle << "\n";
  out << "cancel_drain_last_scheduled_executor_tag="
      << run.cancel_drain1.task.last_scheduled_executor_tag << "\n";
  out << "cancel_drain_last_cancelled_task_handle="
      << run.cancel_drain1.task.last_cancelled_task_handle << "\n";
  out << "cancel_drain_last_cancelled_executor_tag="
      << run.cancel_drain1.task.last_cancelled_executor_tag << "\n";
  out << "cancel_drain_last_executor_queue_depth="
      << run.cancel_drain1.task.last_executor_queue_depth << "\n";
  out << "cancel_drain_max_executor_queue_depth="
      << run.cancel_drain1.task.max_executor_queue_depth << "\n";
  out << "cancel_drain_scheduler_sequence="
      << run.cancel_drain1.task.scheduler_sequence << "\n";
  out << "cancel_drain_deadlock_guard_passed="
      << run.cancel_drain1.task.deadlock_guard_passed << "\n";
  out << "cancel_drain_race_guard_passed="
      << run.cancel_drain1.task.race_guard_passed << "\n";
  out << "cancel_drain_replay_equal="
      << (Equivalent(run.cancel_drain1, run.cancel_drain2) ? 1 : 0)
      << "\n";
  out << "invalid_replay_equal="
      << (Equivalent(run.invalid1, run.invalid2) ? 1 : 0) << "\n";
  out << "replay_equal=" << (Equivalent(run.pass1, run.pass2) ? 1 : 0)
      << "\n";
}

inline void WriteProbeReportToStdout(const ProbeRun &run) {
  WriteProbeReport(run, std::cout);
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
