#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_async_continuation_state_snapshot {
  uint64_t allocation_call_count;
  uint64_t handoff_call_count;
  uint64_t resume_call_count;
  uint64_t cancel_call_count;
  uint64_t rejected_operation_count;
  uint64_t live_continuation_handle_count;
  uint64_t completed_continuation_count;
  uint64_t cancelled_continuation_count;
  uint64_t failed_continuation_count;
  int last_allocated_continuation_handle;
  int last_allocated_continuation_slot;
  int last_allocated_continuation_generation;
  int last_allocated_resume_entry_tag;
  int last_allocated_executor_tag;
  int last_handoff_continuation_handle;
  int last_handoff_executor_tag;
  int last_resume_continuation_handle;
  int last_resume_result_value;
  int last_resume_return_value;
  int last_cancel_continuation_handle;
  int last_cancel_return_value;
  int last_operation_failure_code;
  int last_observed_continuation_slot;
  int last_observed_continuation_generation;
} objc3_runtime_async_continuation_state_snapshot;

enum {
  OBJC3_RUNTIME_TASK_FAILURE_NONE = 0,
  OBJC3_RUNTIME_TASK_FAILURE_INVALID_EXECUTOR = 1,
  OBJC3_RUNTIME_TASK_FAILURE_UNSUPPORTED_TASK_KIND = 2,
  OBJC3_RUNTIME_TASK_FAILURE_MISSING_TASK_GROUP = 3,
  OBJC3_RUNTIME_TASK_FAILURE_TASK_GROUP_ALREADY_ACTIVE = 4,
  OBJC3_RUNTIME_TASK_FAILURE_EXECUTOR_MISMATCH = 5,
  OBJC3_RUNTIME_TASK_FAILURE_EMPTY_TASK_GROUP_QUEUE = 6,
  OBJC3_RUNTIME_TASK_FAILURE_TASK_GROUP_ALREADY_CANCELLED = 7,
};

enum {
  OBJC3_RUNTIME_TASK_LIFECYCLE_IDLE = 0,
  OBJC3_RUNTIME_TASK_LIFECYCLE_TASK_SPAWNED = 1,
  OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_ACTIVE = 2,
  OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_DRAINED = 3,
  OBJC3_RUNTIME_TASK_LIFECYCLE_GROUP_CANCELLED = 4,
};

typedef struct objc3_runtime_task_runtime_state_snapshot {
  uint64_t spawn_call_count;
  uint64_t scope_call_count;
  uint64_t add_task_call_count;
  uint64_t wait_next_call_count;
  uint64_t cancel_all_call_count;
  uint64_t cancellation_poll_call_count;
  uint64_t on_cancel_call_count;
  uint64_t executor_hop_call_count;
  int last_spawn_kind;
  int last_spawn_executor_tag;
  int last_scope_executor_tag;
  int last_add_task_executor_tag;
  int last_wait_next_executor_tag;
  int last_cancel_all_executor_tag;
  int last_cancellation_poll_executor_tag;
  int last_on_cancel_executor_tag;
  int last_executor_hop_executor_tag;
  int last_executor_hop_value;
  int last_wait_next_result;
  int last_cancel_all_result;
  int last_cancellation_poll_result;
  int last_failure_reason;
  int lifecycle_state;
  int selected_executor_tag;
  int active_group_executor_tag;
  int active_group_task_count;
  int pending_group_task_count;
  int completed_group_task_count;
  int group_cancelled;
  int cancellation_generation;
  int observed_cancellation_generation;
  int last_queue_depth;
  int last_queue_drain_result;
} objc3_runtime_task_runtime_state_snapshot;

typedef struct objc3_runtime_actor_runtime_state_snapshot {
  uint64_t isolation_thunk_call_count;
  uint64_t nonisolated_entry_call_count;
  uint64_t hop_to_executor_call_count;
  uint64_t replay_proof_call_count;
  uint64_t race_guard_call_count;
  uint64_t bind_executor_call_count;
  uint64_t mailbox_enqueue_call_count;
  uint64_t mailbox_drain_call_count;
  uint64_t failed_operation_count;
  int last_isolation_executor_tag;
  int last_nonisolated_value;
  int last_nonisolated_executor_tag;
  int last_hop_value;
  int last_hop_executor_tag;
  int last_hop_result;
  int last_replay_proof_executor_tag;
  int last_race_guard_executor_tag;
  int last_bound_actor_handle;
  int last_bound_executor_tag;
  int last_mailbox_actor_handle;
  int last_mailbox_enqueued_value;
  int last_mailbox_executor_tag;
  int last_mailbox_depth;
  int last_mailbox_drained_value;
  int last_operation_succeeded;
  int last_failure_code;
} objc3_runtime_actor_runtime_state_snapshot;

enum {
  OBJC3_RUNTIME_ACTOR_FAILURE_NONE = 0,
  OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_EXECUTOR = 1,
  OBJC3_RUNTIME_ACTOR_FAILURE_INVALID_ACTOR_HANDLE = 2,
  OBJC3_RUNTIME_ACTOR_FAILURE_EMPTY_MAILBOX = 3,
};

// actor lowering/runtime anchor: actor thunk, nonisolated entry,
// and executor-hop lowering remain private runtime helpers with a private
// testing snapshot rather than a public actor runtime ABI.
// actor-runtime/executor-binding anchor: the same private actor
// helper cluster plus `objc3_runtime_actor_runtime_state_snapshot` is now the
// canonical lane-D runtime contract for actor-state, mailbox-ownership, and
// executor-binding proof without widening the public runtime header.
// actor-mailbox/isolation-runtime anchor: live mailbox binding,
// enqueue, and drain helpers also remain inside that same private snapshot-
// backed runtime slice rather than claiming a public actor runtime ABI.
// cross-module isolation-metadata hardening anchor: imported modules
// now preserve the replay facts that describe this same private actor mailbox
// runtime slice across runtime-import surfaces and mixed-module link plans.

// continuation/runtime-helper anchor: lane-D now freezes the first
// truthful private Part 7 helper ABI for logical continuation-handle
// allocation, scheduler handoff, and resume traffic. The current direct-call
// async slice still does not consume this helper cluster yet, but the helper
// runtime surface itself is now real and probeable without widening the public
// runtime header.
int objc3_runtime_allocate_async_continuation_i32(int resume_entry_tag,
                                                  int executor_tag);
int objc3_runtime_handoff_async_continuation_to_executor_i32(
    int continuation_handle, int executor_tag);
int objc3_runtime_resume_async_continuation_i32(int continuation_handle,
                                                int result_value);
int objc3_runtime_cancel_async_continuation_i32(int continuation_handle);
// task-runtime lowering anchor: the IR emitter now rewrites the
// supported task/executor/cancellation symbol-profile family onto this private
// helper cluster so task creation, task-group operations, cancellation polls,
// and executor-handoff proof points become real runnable runtime traffic
// without widening the public runtime header.
// scheduler/executor runtime anchor: lane-D now freezes this same
// private helper cluster plus `objc3_runtime_copy_task_runtime_state_for_testing`
// as the canonical scheduler/executor/task-state runtime contract for the
// current supported Part 7 slice.
// live task runtime anchor: the same private helper cluster now also
// serves as the executable Part 7 runtime boundary for the supported task
// slice, with live probe coverage proving helper traffic and snapshot state.
int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag);
int objc3_runtime_enter_task_group_scope_i32(int executor_tag);
int objc3_runtime_add_task_group_task_i32(int executor_tag);
int objc3_runtime_wait_task_group_next_i32(int executor_tag);
int objc3_runtime_cancel_task_group_i32(int executor_tag);
int objc3_runtime_task_is_cancelled_i32(int executor_tag);
int objc3_runtime_task_on_cancel_i32(int executor_tag);
int objc3_runtime_executor_hop_i32(int value, int executor_tag);
int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag);
int objc3_runtime_actor_enter_nonisolated_i32(int value, int executor_tag);
int objc3_runtime_actor_hop_to_executor_i32(int value, int executor_tag);
int objc3_runtime_actor_record_replay_proof_i32(int executor_tag);
int objc3_runtime_actor_record_race_guard_i32(int executor_tag);
int objc3_runtime_actor_bind_executor_i32(int actor_handle, int executor_tag);
int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle, int value,
                                            int executor_tag);
int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                               int executor_tag);
// live task runtime anchor: task-runtime snapshot publication stays
// private and is consumed by the linked runtime probe rather than a widened
// public scheduler ABI.
// hardening anchor: cancellation cleanup, autoreleasepool scopes,
// arc-debug counters, and explicit runtime resets are validated against the
// same private snapshot/testing surface rather than widening any public task
// scheduler ABI.
int objc3_runtime_copy_async_continuation_state_for_testing(
    objc3_runtime_async_continuation_state_snapshot *snapshot);
int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot);
int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
