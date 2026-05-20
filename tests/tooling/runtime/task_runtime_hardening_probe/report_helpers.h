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
  out << "pass1_after_wait_copy_status="
      << pass1.after_wait_copy_status << "\n";
  out << "pass1_copy_memory_status=" << pass1.copy_memory_status << "\n";
  out << "pass1_copy_arc_status=" << pass1.copy_arc_status << "\n";
  out << "pass1_spawn_group=" << pass1.spawn_group << "\n";
  out << "pass1_wait_next=" << pass1.wait_next << "\n";
  out << "pass1_cancel_all=" << pass1.cancel_all << "\n";
  out << "pass1_spawn_call_count=" << pass1.task.spawn_call_count << "\n";
  out << "pass1_cancel_all_call_count="
      << pass1.task.cancel_all_call_count << "\n";
  out << "pass1_executor_hop_call_count="
      << pass1.task.executor_hop_call_count << "\n";
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
  out << "pass1_after_wait_last_queue_depth="
      << pass1.after_wait_next.last_queue_depth << "\n";
  out << "pass1_after_wait_last_queue_drain_result="
      << pass1.after_wait_next.last_queue_drain_result << "\n";
  out << "pass1_after_wait_scheduler_dequeue_count="
      << pass1.after_wait_next.scheduler_dequeue_count << "\n";
  out << "pass1_after_wait_last_dequeued_task_handle="
      << pass1.after_wait_next.last_dequeued_task_handle << "\n";
  out << "pass1_after_wait_last_scheduled_executor_tag="
      << pass1.after_wait_next.last_scheduled_executor_tag << "\n";
  out << "pass1_after_wait_last_dequeued_executor_tag="
      << pass1.after_wait_next.last_dequeued_executor_tag << "\n";
  out << "pass1_after_wait_deadlock_guard_passed="
      << pass1.after_wait_next.deadlock_guard_passed << "\n";
  out << "pass1_after_wait_race_guard_passed="
      << pass1.after_wait_next.race_guard_passed << "\n";
  out << "pass1_last_failure_reason="
      << pass1.task.last_failure_reason << "\n";
  out << "pass1_lifecycle_state=" << pass1.task.lifecycle_state << "\n";
  out << "pass1_selected_executor_tag="
      << pass1.task.selected_executor_tag << "\n";
  out << "pass1_active_group_executor_tag="
      << pass1.task.active_group_executor_tag << "\n";
  out << "pass1_pending_group_task_count="
      << pass1.task.pending_group_task_count << "\n";
  out << "pass1_completed_group_task_count="
      << pass1.task.completed_group_task_count << "\n";
  out << "pass1_group_cancelled=" << pass1.task.group_cancelled << "\n";
  out << "pass1_cancellation_generation="
      << pass1.task.cancellation_generation << "\n";
  out << "pass1_last_queue_drain_result="
      << pass1.task.last_queue_drain_result << "\n";
  out << "pass1_scheduler_enqueue_count="
      << pass1.task.scheduler_enqueue_count << "\n";
  out << "pass1_scheduler_dequeue_count="
      << pass1.task.scheduler_dequeue_count << "\n";
  out << "pass1_last_scheduled_task_handle="
      << pass1.task.last_scheduled_task_handle << "\n";
  out << "pass1_last_scheduled_executor_tag="
      << pass1.task.last_scheduled_executor_tag << "\n";
  out << "pass1_last_dequeued_task_handle="
      << pass1.task.last_dequeued_task_handle << "\n";
  out << "pass1_last_dequeued_executor_tag="
      << pass1.task.last_dequeued_executor_tag << "\n";
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
  out << "invalid1_deadlock_guard_passed="
      << run.invalid1.task.deadlock_guard_passed << "\n";
  out << "invalid1_race_guard_passed="
      << run.invalid1.task.race_guard_passed << "\n";
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
