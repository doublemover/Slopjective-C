#pragma once

#include "probe_state.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline bool Equivalent(const PassResult &lhs, const PassResult &rhs) {
  return lhs.spawn_group == rhs.spawn_group && lhs.scope == rhs.scope &&
         lhs.add_task == rhs.add_task &&
         lhs.add_second_task == rhs.add_second_task &&
         lhs.cancelled == rhs.cancelled &&
         lhs.wait_next == rhs.wait_next &&
         lhs.wait_second_next == rhs.wait_second_next &&
         lhs.hop == rhs.hop &&
         lhs.cancel_all == rhs.cancel_all && lhs.on_cancel == rhs.on_cancel &&
         lhs.spawn_detached == rhs.spawn_detached &&
         lhs.after_add_copy_status == rhs.after_add_copy_status &&
         lhs.after_second_add_copy_status ==
             rhs.after_second_add_copy_status &&
         lhs.after_wait_copy_status == rhs.after_wait_copy_status &&
         lhs.after_second_wait_copy_status ==
             rhs.after_second_wait_copy_status &&
         lhs.copy_task_status == rhs.copy_task_status &&
         lhs.copy_memory_status == rhs.copy_memory_status &&
         lhs.copy_arc_status == rhs.copy_arc_status &&
         lhs.task.spawn_call_count == rhs.task.spawn_call_count &&
         lhs.task.scope_call_count == rhs.task.scope_call_count &&
         lhs.task.add_task_call_count == rhs.task.add_task_call_count &&
         lhs.task.wait_next_call_count == rhs.task.wait_next_call_count &&
         lhs.task.cancel_all_call_count == rhs.task.cancel_all_call_count &&
         lhs.task.cancellation_poll_call_count ==
             rhs.task.cancellation_poll_call_count &&
         lhs.task.on_cancel_call_count == rhs.task.on_cancel_call_count &&
         lhs.task.executor_hop_call_count == rhs.task.executor_hop_call_count &&
         lhs.after_add_task.last_queue_depth ==
             rhs.after_add_task.last_queue_depth &&
         lhs.after_add_task.scheduler_enqueue_count ==
             rhs.after_add_task.scheduler_enqueue_count &&
         lhs.after_add_task.scheduler_dequeue_count ==
             rhs.after_add_task.scheduler_dequeue_count &&
         lhs.after_add_task.last_scheduled_task_handle ==
             rhs.after_add_task.last_scheduled_task_handle &&
         lhs.after_add_task.last_scheduled_executor_tag ==
             rhs.after_add_task.last_scheduled_executor_tag &&
         lhs.after_add_task.last_executor_queue_depth ==
             rhs.after_add_task.last_executor_queue_depth &&
         lhs.after_add_task.max_executor_queue_depth ==
             rhs.after_add_task.max_executor_queue_depth &&
         lhs.after_add_task.scheduler_sequence ==
             rhs.after_add_task.scheduler_sequence &&
         lhs.after_add_task.deadlock_guard_passed ==
             rhs.after_add_task.deadlock_guard_passed &&
         lhs.after_add_task.race_guard_passed ==
             rhs.after_add_task.race_guard_passed &&
         lhs.after_second_add_task.last_queue_depth ==
             rhs.after_second_add_task.last_queue_depth &&
         lhs.after_second_add_task.scheduler_enqueue_count ==
             rhs.after_second_add_task.scheduler_enqueue_count &&
         lhs.after_second_add_task.scheduler_dequeue_count ==
             rhs.after_second_add_task.scheduler_dequeue_count &&
         lhs.after_second_add_task.last_scheduled_task_handle ==
             rhs.after_second_add_task.last_scheduled_task_handle &&
         lhs.after_second_add_task.last_scheduled_executor_tag ==
             rhs.after_second_add_task.last_scheduled_executor_tag &&
         lhs.after_second_add_task.last_executor_queue_depth ==
             rhs.after_second_add_task.last_executor_queue_depth &&
         lhs.after_second_add_task.max_executor_queue_depth ==
             rhs.after_second_add_task.max_executor_queue_depth &&
         lhs.after_second_add_task.scheduler_sequence ==
             rhs.after_second_add_task.scheduler_sequence &&
         lhs.after_second_add_task.deadlock_guard_passed ==
             rhs.after_second_add_task.deadlock_guard_passed &&
         lhs.after_second_add_task.race_guard_passed ==
             rhs.after_second_add_task.race_guard_passed &&
         lhs.after_wait_next.last_queue_depth ==
             rhs.after_wait_next.last_queue_depth &&
         lhs.after_wait_next.last_queue_drain_result ==
             rhs.after_wait_next.last_queue_drain_result &&
         lhs.after_wait_next.scheduler_enqueue_count ==
             rhs.after_wait_next.scheduler_enqueue_count &&
         lhs.after_wait_next.scheduler_dequeue_count ==
             rhs.after_wait_next.scheduler_dequeue_count &&
         lhs.after_wait_next.last_scheduled_task_handle ==
             rhs.after_wait_next.last_scheduled_task_handle &&
         lhs.after_wait_next.last_scheduled_executor_tag ==
             rhs.after_wait_next.last_scheduled_executor_tag &&
         lhs.after_wait_next.last_dequeued_task_handle ==
             rhs.after_wait_next.last_dequeued_task_handle &&
         lhs.after_wait_next.last_dequeued_executor_tag ==
             rhs.after_wait_next.last_dequeued_executor_tag &&
         lhs.after_wait_next.last_executor_queue_depth ==
             rhs.after_wait_next.last_executor_queue_depth &&
         lhs.after_wait_next.max_executor_queue_depth ==
             rhs.after_wait_next.max_executor_queue_depth &&
         lhs.after_wait_next.scheduler_sequence ==
             rhs.after_wait_next.scheduler_sequence &&
         lhs.after_wait_next.deadlock_guard_passed ==
             rhs.after_wait_next.deadlock_guard_passed &&
         lhs.after_wait_next.race_guard_passed ==
             rhs.after_wait_next.race_guard_passed &&
         lhs.after_second_wait_next.last_queue_depth ==
             rhs.after_second_wait_next.last_queue_depth &&
         lhs.after_second_wait_next.last_queue_drain_result ==
             rhs.after_second_wait_next.last_queue_drain_result &&
         lhs.after_second_wait_next.scheduler_enqueue_count ==
             rhs.after_second_wait_next.scheduler_enqueue_count &&
         lhs.after_second_wait_next.scheduler_dequeue_count ==
             rhs.after_second_wait_next.scheduler_dequeue_count &&
         lhs.after_second_wait_next.last_scheduled_task_handle ==
             rhs.after_second_wait_next.last_scheduled_task_handle &&
         lhs.after_second_wait_next.last_scheduled_executor_tag ==
             rhs.after_second_wait_next.last_scheduled_executor_tag &&
         lhs.after_second_wait_next.last_dequeued_task_handle ==
             rhs.after_second_wait_next.last_dequeued_task_handle &&
         lhs.after_second_wait_next.last_dequeued_executor_tag ==
             rhs.after_second_wait_next.last_dequeued_executor_tag &&
         lhs.after_second_wait_next.last_executor_queue_depth ==
             rhs.after_second_wait_next.last_executor_queue_depth &&
         lhs.after_second_wait_next.max_executor_queue_depth ==
             rhs.after_second_wait_next.max_executor_queue_depth &&
         lhs.after_second_wait_next.scheduler_sequence ==
             rhs.after_second_wait_next.scheduler_sequence &&
         lhs.after_second_wait_next.deadlock_guard_passed ==
             rhs.after_second_wait_next.deadlock_guard_passed &&
         lhs.after_second_wait_next.race_guard_passed ==
             rhs.after_second_wait_next.race_guard_passed &&
         lhs.task.last_spawn_kind == rhs.task.last_spawn_kind &&
         lhs.task.last_spawn_executor_tag == rhs.task.last_spawn_executor_tag &&
         lhs.task.last_wait_next_result == rhs.task.last_wait_next_result &&
         lhs.task.last_executor_hop_executor_tag ==
             rhs.task.last_executor_hop_executor_tag &&
         lhs.task.last_executor_hop_value ==
             rhs.task.last_executor_hop_value &&
         lhs.task.last_failure_reason == rhs.task.last_failure_reason &&
         lhs.task.lifecycle_state == rhs.task.lifecycle_state &&
         lhs.task.selected_executor_tag == rhs.task.selected_executor_tag &&
         lhs.task.active_group_executor_tag ==
             rhs.task.active_group_executor_tag &&
         lhs.task.active_group_task_count ==
             rhs.task.active_group_task_count &&
         lhs.task.pending_group_task_count ==
             rhs.task.pending_group_task_count &&
         lhs.task.completed_group_task_count ==
             rhs.task.completed_group_task_count &&
         lhs.task.cancelled_group_task_count ==
             rhs.task.cancelled_group_task_count &&
         lhs.task.group_cancelled == rhs.task.group_cancelled &&
         lhs.task.cancellation_generation ==
             rhs.task.cancellation_generation &&
         lhs.task.observed_cancellation_generation ==
             rhs.task.observed_cancellation_generation &&
         lhs.task.last_queue_depth == rhs.task.last_queue_depth &&
         lhs.task.last_queue_drain_result ==
             rhs.task.last_queue_drain_result &&
         lhs.task.scheduler_enqueue_count ==
             rhs.task.scheduler_enqueue_count &&
         lhs.task.scheduler_dequeue_count ==
             rhs.task.scheduler_dequeue_count &&
         lhs.task.scheduler_cancelled_count ==
             rhs.task.scheduler_cancelled_count &&
         lhs.task.last_scheduled_task_handle ==
             rhs.task.last_scheduled_task_handle &&
         lhs.task.last_scheduled_executor_tag ==
             rhs.task.last_scheduled_executor_tag &&
         lhs.task.last_dequeued_task_handle ==
             rhs.task.last_dequeued_task_handle &&
         lhs.task.last_dequeued_executor_tag ==
             rhs.task.last_dequeued_executor_tag &&
         lhs.task.last_cancelled_task_handle ==
             rhs.task.last_cancelled_task_handle &&
         lhs.task.last_cancelled_executor_tag ==
             rhs.task.last_cancelled_executor_tag &&
         lhs.task.last_executor_queue_depth ==
             rhs.task.last_executor_queue_depth &&
         lhs.task.max_executor_queue_depth ==
             rhs.task.max_executor_queue_depth &&
         lhs.task.scheduler_sequence == rhs.task.scheduler_sequence &&
         lhs.task.deadlock_guard_passed ==
             rhs.task.deadlock_guard_passed &&
         lhs.task.race_guard_passed == rhs.task.race_guard_passed &&
         lhs.memory.autoreleasepool_depth ==
             rhs.memory.autoreleasepool_depth &&
         lhs.memory.autoreleasepool_max_depth ==
             rhs.memory.autoreleasepool_max_depth &&
         lhs.arc.autoreleasepool_push_count ==
             rhs.arc.autoreleasepool_push_count &&
         lhs.arc.autoreleasepool_pop_count ==
             rhs.arc.autoreleasepool_pop_count;
}

inline bool Equivalent(const InvalidHandleResult &lhs,
                       const InvalidHandleResult &rhs) {
  return lhs.invalid_spawn_kind == rhs.invalid_spawn_kind &&
         lhs.invalid_spawn_executor == rhs.invalid_spawn_executor &&
         lhs.missing_group_add == rhs.missing_group_add &&
         lhs.missing_group_wait == rhs.missing_group_wait &&
         lhs.missing_group_cancel == rhs.missing_group_cancel &&
         lhs.scope == rhs.scope &&
         lhs.executor_mismatch_add == rhs.executor_mismatch_add &&
         lhs.copy_task_status == rhs.copy_task_status &&
         lhs.task.spawn_call_count == rhs.task.spawn_call_count &&
         lhs.task.scope_call_count == rhs.task.scope_call_count &&
         lhs.task.add_task_call_count == rhs.task.add_task_call_count &&
         lhs.task.wait_next_call_count == rhs.task.wait_next_call_count &&
         lhs.task.cancel_all_call_count == rhs.task.cancel_all_call_count &&
         lhs.task.last_failure_reason == rhs.task.last_failure_reason &&
         lhs.task.lifecycle_state == rhs.task.lifecycle_state &&
         lhs.task.selected_executor_tag == rhs.task.selected_executor_tag &&
         lhs.task.active_group_executor_tag ==
             rhs.task.active_group_executor_tag &&
         lhs.task.active_group_task_count ==
             rhs.task.active_group_task_count &&
         lhs.task.pending_group_task_count ==
             rhs.task.pending_group_task_count &&
         lhs.task.completed_group_task_count ==
             rhs.task.completed_group_task_count &&
         lhs.task.cancelled_group_task_count ==
             rhs.task.cancelled_group_task_count &&
         lhs.task.group_cancelled == rhs.task.group_cancelled &&
         lhs.task.last_wait_next_result == rhs.task.last_wait_next_result &&
         lhs.task.last_cancel_all_result == rhs.task.last_cancel_all_result &&
         lhs.task.scheduler_enqueue_count ==
             rhs.task.scheduler_enqueue_count &&
         lhs.task.scheduler_dequeue_count ==
             rhs.task.scheduler_dequeue_count &&
         lhs.task.scheduler_cancelled_count ==
             rhs.task.scheduler_cancelled_count &&
         lhs.task.last_scheduled_task_handle ==
             rhs.task.last_scheduled_task_handle &&
         lhs.task.last_scheduled_executor_tag ==
             rhs.task.last_scheduled_executor_tag &&
         lhs.task.last_dequeued_task_handle ==
             rhs.task.last_dequeued_task_handle &&
         lhs.task.last_dequeued_executor_tag ==
             rhs.task.last_dequeued_executor_tag &&
         lhs.task.last_cancelled_task_handle ==
             rhs.task.last_cancelled_task_handle &&
         lhs.task.last_cancelled_executor_tag ==
             rhs.task.last_cancelled_executor_tag &&
         lhs.task.last_executor_queue_depth ==
             rhs.task.last_executor_queue_depth &&
         lhs.task.max_executor_queue_depth ==
             rhs.task.max_executor_queue_depth &&
         lhs.task.scheduler_sequence == rhs.task.scheduler_sequence &&
         lhs.task.deadlock_guard_passed ==
             rhs.task.deadlock_guard_passed &&
         lhs.task.race_guard_passed == rhs.task.race_guard_passed;
}

inline bool Equivalent(const CancelDrainResult &lhs,
                       const CancelDrainResult &rhs) {
  return lhs.scope == rhs.scope && lhs.add_task == rhs.add_task &&
         lhs.add_second_task == rhs.add_second_task &&
         lhs.cancel_all == rhs.cancel_all &&
         lhs.copy_task_status == rhs.copy_task_status &&
         lhs.task.scope_call_count == rhs.task.scope_call_count &&
         lhs.task.add_task_call_count == rhs.task.add_task_call_count &&
         lhs.task.cancel_all_call_count == rhs.task.cancel_all_call_count &&
         lhs.task.last_cancel_all_result == rhs.task.last_cancel_all_result &&
         lhs.task.last_failure_reason == rhs.task.last_failure_reason &&
         lhs.task.lifecycle_state == rhs.task.lifecycle_state &&
         lhs.task.selected_executor_tag == rhs.task.selected_executor_tag &&
         lhs.task.active_group_executor_tag ==
             rhs.task.active_group_executor_tag &&
         lhs.task.active_group_task_count ==
             rhs.task.active_group_task_count &&
         lhs.task.pending_group_task_count ==
             rhs.task.pending_group_task_count &&
         lhs.task.completed_group_task_count ==
             rhs.task.completed_group_task_count &&
         lhs.task.cancelled_group_task_count ==
             rhs.task.cancelled_group_task_count &&
         lhs.task.group_cancelled == rhs.task.group_cancelled &&
         lhs.task.cancellation_generation ==
             rhs.task.cancellation_generation &&
         lhs.task.last_queue_depth == rhs.task.last_queue_depth &&
         lhs.task.last_queue_drain_result ==
             rhs.task.last_queue_drain_result &&
         lhs.task.scheduler_enqueue_count ==
             rhs.task.scheduler_enqueue_count &&
         lhs.task.scheduler_dequeue_count ==
             rhs.task.scheduler_dequeue_count &&
         lhs.task.scheduler_cancelled_count ==
             rhs.task.scheduler_cancelled_count &&
         lhs.task.last_scheduled_task_handle ==
             rhs.task.last_scheduled_task_handle &&
         lhs.task.last_scheduled_executor_tag ==
             rhs.task.last_scheduled_executor_tag &&
         lhs.task.last_cancelled_task_handle ==
             rhs.task.last_cancelled_task_handle &&
         lhs.task.last_cancelled_executor_tag ==
             rhs.task.last_cancelled_executor_tag &&
         lhs.task.last_executor_queue_depth ==
             rhs.task.last_executor_queue_depth &&
         lhs.task.max_executor_queue_depth ==
             rhs.task.max_executor_queue_depth &&
         lhs.task.scheduler_sequence == rhs.task.scheduler_sequence &&
         lhs.task.deadlock_guard_passed ==
             rhs.task.deadlock_guard_passed &&
         lhs.task.race_guard_passed == rhs.task.race_guard_passed;
}

inline bool Equivalent(const ExecutorHopRaceGuardResult &lhs,
                       const ExecutorHopRaceGuardResult &rhs) {
  return lhs.scope == rhs.scope && lhs.add_task == rhs.add_task &&
         lhs.wait_next == rhs.wait_next &&
         lhs.stale_value_hop == rhs.stale_value_hop &&
         lhs.wrong_executor_hop == rhs.wrong_executor_hop &&
         lhs.copy_task_status == rhs.copy_task_status &&
         lhs.task.scope_call_count == rhs.task.scope_call_count &&
         lhs.task.add_task_call_count == rhs.task.add_task_call_count &&
         lhs.task.wait_next_call_count == rhs.task.wait_next_call_count &&
         lhs.task.executor_hop_call_count ==
             rhs.task.executor_hop_call_count &&
         lhs.task.last_failure_reason == rhs.task.last_failure_reason &&
         lhs.task.lifecycle_state == rhs.task.lifecycle_state &&
         lhs.task.selected_executor_tag == rhs.task.selected_executor_tag &&
         lhs.task.active_group_executor_tag ==
             rhs.task.active_group_executor_tag &&
         lhs.task.active_group_task_count ==
             rhs.task.active_group_task_count &&
         lhs.task.pending_group_task_count ==
             rhs.task.pending_group_task_count &&
         lhs.task.completed_group_task_count ==
             rhs.task.completed_group_task_count &&
         lhs.task.cancelled_group_task_count ==
             rhs.task.cancelled_group_task_count &&
         lhs.task.group_cancelled == rhs.task.group_cancelled &&
         lhs.task.last_wait_next_result == rhs.task.last_wait_next_result &&
         lhs.task.last_executor_hop_executor_tag ==
             rhs.task.last_executor_hop_executor_tag &&
         lhs.task.last_executor_hop_value ==
             rhs.task.last_executor_hop_value &&
         lhs.task.last_queue_depth == rhs.task.last_queue_depth &&
         lhs.task.last_queue_drain_result ==
             rhs.task.last_queue_drain_result &&
         lhs.task.scheduler_enqueue_count ==
             rhs.task.scheduler_enqueue_count &&
         lhs.task.scheduler_dequeue_count ==
             rhs.task.scheduler_dequeue_count &&
         lhs.task.scheduler_cancelled_count ==
             rhs.task.scheduler_cancelled_count &&
         lhs.task.last_scheduled_task_handle ==
             rhs.task.last_scheduled_task_handle &&
         lhs.task.last_scheduled_executor_tag ==
             rhs.task.last_scheduled_executor_tag &&
         lhs.task.last_dequeued_task_handle ==
             rhs.task.last_dequeued_task_handle &&
         lhs.task.last_dequeued_executor_tag ==
             rhs.task.last_dequeued_executor_tag &&
         lhs.task.last_executor_queue_depth ==
             rhs.task.last_executor_queue_depth &&
         lhs.task.max_executor_queue_depth ==
             rhs.task.max_executor_queue_depth &&
         lhs.task.scheduler_sequence == rhs.task.scheduler_sequence &&
         lhs.task.deadlock_guard_passed ==
             rhs.task.deadlock_guard_passed &&
         lhs.task.race_guard_passed == rhs.task.race_guard_passed;
}

inline bool SnapshotCopiesSucceeded(const PassResult &pass) {
  return pass.after_add_copy_status == 0 &&
         pass.after_second_add_copy_status == 0 &&
         pass.after_wait_copy_status == 0 &&
         pass.after_second_wait_copy_status == 0 &&
         pass.copy_task_status == 0 && pass.copy_memory_status == 0 &&
         pass.copy_arc_status == 0;
}

inline bool TaskRuntimeCallsReturnExpectedValues(const PassResult &pass) {
  return pass.spawn_group == 111 && pass.scope == 1 && pass.add_task == 1 &&
         pass.add_second_task == 1 &&
         pass.cancelled == 0 && pass.wait_next == 23 &&
         pass.wait_second_next == 24 && pass.hop == 23 &&
         pass.cancel_all == 31 && pass.on_cancel == 41 &&
         pass.spawn_detached == 121;
}

inline bool TaskRuntimeCountersMatchExpectedValues(const PassResult &pass) {
  return pass.task.spawn_call_count == 2 && pass.task.scope_call_count == 1 &&
         pass.task.add_task_call_count == 2 &&
         pass.task.wait_next_call_count == 2 &&
         pass.task.cancel_all_call_count == 1 &&
         pass.task.cancellation_poll_call_count == 1 &&
         pass.task.on_cancel_call_count == 1 &&
         pass.task.executor_hop_call_count == 1 &&
         pass.task.last_spawn_kind == 2 &&
         pass.task.last_spawn_executor_tag == 3 &&
         pass.task.last_wait_next_result == 24 &&
         pass.task.last_executor_hop_executor_tag == 2 &&
         pass.task.last_executor_hop_value == 23 &&
         pass.after_add_task.last_queue_depth == 1 &&
         pass.after_add_task.scheduler_enqueue_count == 1 &&
         pass.after_add_task.scheduler_dequeue_count == 0 &&
         pass.after_add_task.last_scheduled_task_handle == 23 &&
         pass.after_add_task.last_scheduled_executor_tag == 2 &&
         pass.after_add_task.last_executor_queue_depth == 1 &&
         pass.after_add_task.max_executor_queue_depth == 1 &&
         pass.after_add_task.scheduler_sequence == 1 &&
         pass.after_add_task.deadlock_guard_passed == 1 &&
         pass.after_add_task.race_guard_passed == 1 &&
         pass.after_second_add_task.last_queue_depth == 2 &&
         pass.after_second_add_task.scheduler_enqueue_count == 2 &&
         pass.after_second_add_task.scheduler_dequeue_count == 0 &&
         pass.after_second_add_task.last_scheduled_task_handle == 24 &&
         pass.after_second_add_task.last_scheduled_executor_tag == 2 &&
         pass.after_second_add_task.last_executor_queue_depth == 2 &&
         pass.after_second_add_task.max_executor_queue_depth == 2 &&
         pass.after_second_add_task.scheduler_sequence == 2 &&
         pass.after_second_add_task.deadlock_guard_passed == 1 &&
         pass.after_second_add_task.race_guard_passed == 1 &&
         pass.after_wait_next.last_queue_depth == 1 &&
         pass.after_wait_next.last_queue_drain_result == 23 &&
         pass.after_wait_next.scheduler_enqueue_count == 2 &&
         pass.after_wait_next.scheduler_dequeue_count == 1 &&
         pass.after_wait_next.last_scheduled_task_handle == 24 &&
         pass.after_wait_next.last_scheduled_executor_tag == 2 &&
         pass.after_wait_next.last_dequeued_task_handle == 23 &&
         pass.after_wait_next.last_dequeued_executor_tag == 2 &&
         pass.after_wait_next.last_executor_queue_depth == 1 &&
         pass.after_wait_next.max_executor_queue_depth == 2 &&
         pass.after_wait_next.scheduler_sequence == 3 &&
         pass.after_wait_next.deadlock_guard_passed == 1 &&
         pass.after_wait_next.race_guard_passed == 1 &&
         pass.after_second_wait_next.last_queue_depth == 0 &&
         pass.after_second_wait_next.last_queue_drain_result == 24 &&
         pass.after_second_wait_next.scheduler_enqueue_count == 2 &&
         pass.after_second_wait_next.scheduler_dequeue_count == 2 &&
         pass.after_second_wait_next.last_scheduled_task_handle == 24 &&
         pass.after_second_wait_next.last_scheduled_executor_tag == 2 &&
         pass.after_second_wait_next.last_dequeued_task_handle == 24 &&
         pass.after_second_wait_next.last_dequeued_executor_tag == 2 &&
         pass.after_second_wait_next.last_executor_queue_depth == 0 &&
         pass.after_second_wait_next.max_executor_queue_depth == 2 &&
         pass.after_second_wait_next.scheduler_sequence == 4 &&
         pass.after_second_wait_next.deadlock_guard_passed == 1 &&
         pass.after_second_wait_next.race_guard_passed == 1 &&
         pass.task.last_failure_reason == OBJC3_RUNTIME_TASK_FAILURE_NONE &&
         pass.task.lifecycle_state ==
             OBJC3_RUNTIME_TASK_LIFECYCLE_TASK_SPAWNED &&
         pass.task.selected_executor_tag == 3 &&
         pass.task.active_group_executor_tag == 2 &&
         pass.task.active_group_task_count == 2 &&
         pass.task.pending_group_task_count == 0 &&
         pass.task.completed_group_task_count == 2 &&
         pass.task.cancelled_group_task_count == 0 &&
         pass.task.group_cancelled == 1 &&
         pass.task.cancellation_generation == 1 &&
         pass.task.observed_cancellation_generation == 1 &&
         pass.task.last_queue_depth == 0 &&
         pass.task.last_queue_drain_result == 24 &&
         pass.task.scheduler_enqueue_count == 3 &&
         pass.task.scheduler_dequeue_count == 2 &&
         pass.task.scheduler_cancelled_count == 0 &&
         pass.task.last_scheduled_task_handle == 121 &&
         pass.task.last_scheduled_executor_tag == 3 &&
         pass.task.last_dequeued_task_handle == 24 &&
         pass.task.last_dequeued_executor_tag == 2 &&
         pass.task.last_cancelled_task_handle == 0 &&
         pass.task.last_cancelled_executor_tag == 0 &&
         pass.task.last_executor_queue_depth == 1 &&
         pass.task.max_executor_queue_depth == 2 &&
         pass.task.scheduler_sequence == 5 &&
         pass.task.deadlock_guard_passed == 1 &&
         pass.task.race_guard_passed == 1;
}

inline bool MemoryAndArcHardeningCountersMatchExpectedValues(
    const PassResult &pass) {
  return pass.memory.autoreleasepool_depth == 0 &&
         pass.memory.autoreleasepool_max_depth == 1 &&
         pass.arc.autoreleasepool_push_count == 1 &&
         pass.arc.autoreleasepool_pop_count == 1;
}

inline bool PassAssertionsPassed(const PassResult &pass) {
  return SnapshotCopiesSucceeded(pass) &&
         TaskRuntimeCallsReturnExpectedValues(pass) &&
         TaskRuntimeCountersMatchExpectedValues(pass) &&
         MemoryAndArcHardeningCountersMatchExpectedValues(pass);
}

inline bool InvalidHandleAssertionsPassed(const InvalidHandleResult &pass) {
  return pass.copy_task_status == 0 &&
         pass.invalid_spawn_kind ==
             -OBJC3_RUNTIME_TASK_FAILURE_UNSUPPORTED_TASK_KIND &&
         pass.invalid_spawn_executor ==
             -OBJC3_RUNTIME_TASK_FAILURE_INVALID_EXECUTOR &&
         pass.missing_group_add ==
             -OBJC3_RUNTIME_TASK_FAILURE_MISSING_TASK_GROUP &&
         pass.missing_group_wait ==
             -OBJC3_RUNTIME_TASK_FAILURE_MISSING_TASK_GROUP &&
         pass.missing_group_cancel ==
             -OBJC3_RUNTIME_TASK_FAILURE_MISSING_TASK_GROUP &&
         pass.scope == 1 &&
         pass.executor_mismatch_add ==
             -OBJC3_RUNTIME_TASK_FAILURE_EXECUTOR_MISMATCH &&
         pass.task.spawn_call_count == 2 &&
         pass.task.scope_call_count == 1 &&
         pass.task.add_task_call_count == 2 &&
         pass.task.wait_next_call_count == 1 &&
         pass.task.cancel_all_call_count == 1 &&
         pass.task.last_failure_reason ==
             OBJC3_RUNTIME_TASK_FAILURE_EXECUTOR_MISMATCH &&
         pass.task.lifecycle_state ==
             OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_ACTIVE &&
         pass.task.selected_executor_tag == 4 &&
         pass.task.active_group_executor_tag == 4 &&
         pass.task.active_group_task_count == 0 &&
         pass.task.pending_group_task_count == 0 &&
         pass.task.completed_group_task_count == 0 &&
         pass.task.cancelled_group_task_count == 0 &&
         pass.task.group_cancelled == 0 &&
         pass.task.last_wait_next_result ==
             -OBJC3_RUNTIME_TASK_FAILURE_MISSING_TASK_GROUP &&
         pass.task.last_cancel_all_result ==
             -OBJC3_RUNTIME_TASK_FAILURE_MISSING_TASK_GROUP &&
         pass.task.scheduler_enqueue_count == 0 &&
         pass.task.scheduler_dequeue_count == 0 &&
         pass.task.scheduler_cancelled_count == 0 &&
         pass.task.last_scheduled_task_handle == 0 &&
         pass.task.last_dequeued_task_handle == 0 &&
         pass.task.last_cancelled_task_handle == 0 &&
         pass.task.last_cancelled_executor_tag == 0 &&
         pass.task.last_executor_queue_depth == 0 &&
         pass.task.max_executor_queue_depth == 0 &&
         pass.task.scheduler_sequence == 0 &&
         pass.task.deadlock_guard_passed == 1 &&
         pass.task.race_guard_passed == 1;
}

inline bool CancelDrainAssertionsPassed(const CancelDrainResult &pass) {
  return pass.copy_task_status == 0 && pass.scope == 1 &&
         pass.add_task == 1 && pass.add_second_task == 1 &&
         pass.cancel_all == 31 &&
         pass.task.scope_call_count == 1 &&
         pass.task.add_task_call_count == 2 &&
         pass.task.cancel_all_call_count == 1 &&
         pass.task.last_cancel_all_result == 31 &&
         pass.task.last_failure_reason ==
             OBJC3_RUNTIME_TASK_FAILURE_NONE &&
         pass.task.lifecycle_state ==
             OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_CANCELLED &&
         pass.task.selected_executor_tag == 6 &&
         pass.task.active_group_executor_tag == 6 &&
         pass.task.active_group_task_count == 2 &&
         pass.task.pending_group_task_count == 0 &&
         pass.task.completed_group_task_count == 0 &&
         pass.task.cancelled_group_task_count == 2 &&
         pass.task.group_cancelled == 1 &&
         pass.task.cancellation_generation == 1 &&
         pass.task.last_queue_depth == 0 &&
         pass.task.last_queue_drain_result == 28 &&
         pass.task.scheduler_enqueue_count == 2 &&
         pass.task.scheduler_dequeue_count == 0 &&
         pass.task.scheduler_cancelled_count == 2 &&
         pass.task.last_scheduled_task_handle == 28 &&
         pass.task.last_scheduled_executor_tag == 6 &&
         pass.task.last_cancelled_task_handle == 28 &&
         pass.task.last_cancelled_executor_tag == 6 &&
         pass.task.last_executor_queue_depth == 0 &&
         pass.task.max_executor_queue_depth == 2 &&
         pass.task.scheduler_sequence == 4 &&
         pass.task.deadlock_guard_passed == 1 &&
         pass.task.race_guard_passed == 1;
}

inline bool ExecutorHopRaceGuardAssertionsPassed(
    const ExecutorHopRaceGuardResult &pass) {
  return pass.copy_task_status == 0 && pass.scope == 1 &&
         pass.add_task == 1 && pass.wait_next == 28 &&
         pass.stale_value_hop ==
             -OBJC3_RUNTIME_TASK_FAILURE_SCHEDULER_QUEUE_DRIFT &&
         pass.wrong_executor_hop ==
             -OBJC3_RUNTIME_TASK_FAILURE_SCHEDULER_QUEUE_DRIFT &&
         pass.task.scope_call_count == 1 &&
         pass.task.add_task_call_count == 1 &&
         pass.task.wait_next_call_count == 1 &&
         pass.task.executor_hop_call_count == 2 &&
         pass.task.last_failure_reason ==
             OBJC3_RUNTIME_TASK_FAILURE_SCHEDULER_QUEUE_DRIFT &&
         pass.task.lifecycle_state ==
             OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_DRAINED &&
         pass.task.selected_executor_tag == 7 &&
         pass.task.active_group_executor_tag == 7 &&
         pass.task.active_group_task_count == 1 &&
         pass.task.pending_group_task_count == 0 &&
         pass.task.completed_group_task_count == 1 &&
         pass.task.cancelled_group_task_count == 0 &&
         pass.task.group_cancelled == 0 &&
         pass.task.last_wait_next_result == 28 &&
         pass.task.last_executor_hop_executor_tag == 8 &&
         pass.task.last_executor_hop_value == 28 &&
         pass.task.last_queue_depth == 0 &&
         pass.task.last_queue_drain_result == 28 &&
         pass.task.scheduler_enqueue_count == 1 &&
         pass.task.scheduler_dequeue_count == 1 &&
         pass.task.scheduler_cancelled_count == 0 &&
         pass.task.last_scheduled_task_handle == 28 &&
         pass.task.last_scheduled_executor_tag == 7 &&
         pass.task.last_dequeued_task_handle == 28 &&
         pass.task.last_dequeued_executor_tag == 7 &&
         pass.task.last_executor_queue_depth == 0 &&
         pass.task.max_executor_queue_depth == 1 &&
         pass.task.scheduler_sequence == 2 &&
         pass.task.deadlock_guard_passed == 1 &&
         pass.task.race_guard_passed == 0;
}

inline bool ProbeAssertionsPassed(const ProbeRun &run) {
  return PassAssertionsPassed(run.pass1) && Equivalent(run.pass1, run.pass2) &&
         InvalidHandleAssertionsPassed(run.invalid1) &&
         Equivalent(run.invalid1, run.invalid2) &&
         CancelDrainAssertionsPassed(run.cancel_drain1) &&
         Equivalent(run.cancel_drain1, run.cancel_drain2) &&
         ExecutorHopRaceGuardAssertionsPassed(run.hop_race1) &&
         Equivalent(run.hop_race1, run.hop_race2);
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
