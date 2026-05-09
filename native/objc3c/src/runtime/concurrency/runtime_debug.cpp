#include "runtime/concurrency/runtime_debug.h"

#include "runtime/concurrency/actor.h"
#include "runtime/concurrency/executor.h"
#include "runtime/concurrency/task.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <deque>
#include <unordered_map>
#include <unordered_set>

namespace {

thread_local std::uint64_t g_continuation_allocation_call_count = 0;
thread_local std::uint64_t g_continuation_handoff_call_count = 0;
thread_local std::uint64_t g_continuation_resume_call_count = 0;
thread_local int g_continuation_next_handle = 1;
thread_local int g_continuation_last_allocated_handle = 0;
thread_local int g_continuation_last_allocated_resume_entry_tag = 0;
thread_local int g_continuation_last_allocated_executor_tag = 0;
thread_local int g_continuation_last_handoff_handle = 0;
thread_local int g_continuation_last_handoff_executor_tag = 0;
thread_local int g_continuation_last_resume_handle = 0;
thread_local int g_continuation_last_resume_result_value = 0;
thread_local int g_continuation_last_resume_return_value = 0;
thread_local std::unordered_set<int> g_live_continuation_handles;

thread_local std::uint64_t g_task_spawn_call_count = 0;
thread_local std::uint64_t g_task_scope_call_count = 0;
thread_local std::uint64_t g_task_add_task_call_count = 0;
thread_local std::uint64_t g_task_wait_next_call_count = 0;
thread_local std::uint64_t g_task_cancel_all_call_count = 0;
thread_local std::uint64_t g_task_cancellation_poll_call_count = 0;
thread_local std::uint64_t g_task_on_cancel_call_count = 0;
thread_local std::uint64_t g_task_executor_hop_call_count = 0;
thread_local int g_task_last_spawn_kind = 0;
thread_local int g_task_last_spawn_executor_tag = 0;
thread_local int g_task_last_scope_executor_tag = 0;
thread_local int g_task_last_add_task_executor_tag = 0;
thread_local int g_task_last_wait_next_executor_tag = 0;
thread_local int g_task_last_cancel_all_executor_tag = 0;
thread_local int g_task_last_cancellation_poll_executor_tag = 0;
thread_local int g_task_last_on_cancel_executor_tag = 0;
thread_local int g_task_last_executor_hop_executor_tag = 0;
thread_local int g_task_last_executor_hop_value = 0;
thread_local int g_task_last_wait_next_result = 0;
thread_local int g_task_last_cancel_all_result = 0;
thread_local int g_task_last_cancellation_poll_result = 0;

thread_local std::uint64_t g_actor_isolation_thunk_call_count = 0;
thread_local std::uint64_t g_actor_nonisolated_entry_call_count = 0;
thread_local std::uint64_t g_actor_hop_to_executor_call_count = 0;
thread_local std::uint64_t g_actor_replay_proof_call_count = 0;
thread_local std::uint64_t g_actor_race_guard_call_count = 0;
thread_local std::uint64_t g_actor_bind_executor_call_count = 0;
thread_local std::uint64_t g_actor_mailbox_enqueue_call_count = 0;
thread_local std::uint64_t g_actor_mailbox_drain_call_count = 0;
thread_local int g_actor_last_isolation_executor_tag = 0;
thread_local int g_actor_last_nonisolated_value = 0;
thread_local int g_actor_last_nonisolated_executor_tag = 0;
thread_local int g_actor_last_hop_value = 0;
thread_local int g_actor_last_hop_executor_tag = 0;
thread_local int g_actor_last_hop_result = 0;
thread_local int g_actor_last_replay_proof_executor_tag = 0;
thread_local int g_actor_last_race_guard_executor_tag = 0;
thread_local int g_actor_last_bound_actor_handle = 0;
thread_local int g_actor_last_bound_executor_tag = 0;
thread_local int g_actor_last_mailbox_actor_handle = 0;
thread_local int g_actor_last_mailbox_enqueued_value = 0;
thread_local int g_actor_last_mailbox_executor_tag = 0;
thread_local int g_actor_last_mailbox_depth = 0;
thread_local int g_actor_last_mailbox_drained_value = 0;
thread_local std::unordered_map<int, std::deque<int>> g_actor_mailboxes;

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeConcurrencyDebugStateForTesting() {
  g_continuation_allocation_call_count = 0;
  g_continuation_handoff_call_count = 0;
  g_continuation_resume_call_count = 0;
  g_continuation_next_handle = 1;
  g_continuation_last_allocated_handle = 0;
  g_continuation_last_allocated_resume_entry_tag = 0;
  g_continuation_last_allocated_executor_tag = 0;
  g_continuation_last_handoff_handle = 0;
  g_continuation_last_handoff_executor_tag = 0;
  g_continuation_last_resume_handle = 0;
  g_continuation_last_resume_result_value = 0;
  g_continuation_last_resume_return_value = 0;
  g_live_continuation_handles.clear();

  g_task_spawn_call_count = 0;
  g_task_scope_call_count = 0;
  g_task_add_task_call_count = 0;
  g_task_wait_next_call_count = 0;
  g_task_cancel_all_call_count = 0;
  g_task_cancellation_poll_call_count = 0;
  g_task_on_cancel_call_count = 0;
  g_task_executor_hop_call_count = 0;
  g_task_last_spawn_kind = 0;
  g_task_last_spawn_executor_tag = 0;
  g_task_last_scope_executor_tag = 0;
  g_task_last_add_task_executor_tag = 0;
  g_task_last_wait_next_executor_tag = 0;
  g_task_last_cancel_all_executor_tag = 0;
  g_task_last_cancellation_poll_executor_tag = 0;
  g_task_last_on_cancel_executor_tag = 0;
  g_task_last_executor_hop_executor_tag = 0;
  g_task_last_executor_hop_value = 0;
  g_task_last_wait_next_result = 0;
  g_task_last_cancel_all_result = 0;
  g_task_last_cancellation_poll_result = 0;

  g_actor_isolation_thunk_call_count = 0;
  g_actor_nonisolated_entry_call_count = 0;
  g_actor_hop_to_executor_call_count = 0;
  g_actor_replay_proof_call_count = 0;
  g_actor_race_guard_call_count = 0;
  g_actor_bind_executor_call_count = 0;
  g_actor_mailbox_enqueue_call_count = 0;
  g_actor_mailbox_drain_call_count = 0;
  g_actor_last_isolation_executor_tag = 0;
  g_actor_last_nonisolated_value = 0;
  g_actor_last_nonisolated_executor_tag = 0;
  g_actor_last_hop_value = 0;
  g_actor_last_hop_executor_tag = 0;
  g_actor_last_hop_result = 0;
  g_actor_last_replay_proof_executor_tag = 0;
  g_actor_last_race_guard_executor_tag = 0;
  g_actor_last_bound_actor_handle = 0;
  g_actor_last_bound_executor_tag = 0;
  g_actor_last_mailbox_actor_handle = 0;
  g_actor_last_mailbox_enqueued_value = 0;
  g_actor_last_mailbox_executor_tag = 0;
  g_actor_last_mailbox_depth = 0;
  g_actor_last_mailbox_drained_value = 0;
  g_actor_mailboxes.clear();
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_allocate_async_continuation_i32(
    int resume_entry_tag, int executor_tag) {
  ++g_continuation_allocation_call_count;
  const int handle = g_continuation_next_handle++;
  g_continuation_last_allocated_handle = handle;
  g_continuation_last_allocated_resume_entry_tag = resume_entry_tag;
  g_continuation_last_allocated_executor_tag = executor_tag;
  g_live_continuation_handles.insert(handle);
  return handle;
}

extern "C" int objc3_runtime_handoff_async_continuation_to_executor_i32(
    int continuation_handle, int executor_tag) {
  ++g_continuation_handoff_call_count;
  g_continuation_last_handoff_handle = continuation_handle;
  g_continuation_last_handoff_executor_tag = executor_tag;
  if (continuation_handle == 0 ||
      g_live_continuation_handles.find(continuation_handle) ==
          g_live_continuation_handles.end()) {
    return 0;
  }
  return continuation_handle;
}

extern "C" int objc3_runtime_resume_async_continuation_i32(
    int continuation_handle, int result_value) {
  ++g_continuation_resume_call_count;
  g_continuation_last_resume_handle = continuation_handle;
  g_continuation_last_resume_result_value = result_value;
  const auto found = g_live_continuation_handles.find(continuation_handle);
  if (continuation_handle == 0 || found == g_live_continuation_handles.end()) {
    g_continuation_last_resume_return_value = 0;
    return 0;
  }
  g_live_continuation_handles.erase(found);
  g_continuation_last_resume_return_value = result_value;
  return result_value;
}

extern "C" int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag) {
  ++g_task_spawn_call_count;
  g_task_last_spawn_kind = task_kind;
  g_task_last_spawn_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeTaskKindIsSupported(task_kind) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 100 + (task_kind * 10) + (executor_tag != 0 ? 1 : 0);
}

extern "C" int objc3_runtime_enter_task_group_scope_i32(int executor_tag) {
  ++g_task_scope_call_count;
  g_task_last_scope_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

extern "C" int objc3_runtime_add_task_group_task_i32(int executor_tag) {
  ++g_task_add_task_call_count;
  g_task_last_add_task_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 1;
}

extern "C" int objc3_runtime_wait_task_group_next_i32(int executor_tag) {
  ++g_task_wait_next_call_count;
  g_task_last_wait_next_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_task_last_wait_next_result = 0;
    return 0;
  }
  g_task_last_wait_next_result = 23;
  return g_task_last_wait_next_result;
}

extern "C" int objc3_runtime_cancel_task_group_i32(int executor_tag) {
  ++g_task_cancel_all_call_count;
  g_task_last_cancel_all_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_task_last_cancel_all_result = 0;
    return 0;
  }
  g_task_last_cancel_all_result = 31;
  return g_task_last_cancel_all_result;
}

extern "C" int objc3_runtime_task_is_cancelled_i32(int executor_tag) {
  ++g_task_cancellation_poll_call_count;
  g_task_last_cancellation_poll_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_task_last_cancellation_poll_result = 0;
    return 0;
  }
  g_task_last_cancellation_poll_result = 0;
  return g_task_last_cancellation_poll_result;
}

extern "C" int objc3_runtime_task_on_cancel_i32(int executor_tag) {
  ++g_task_on_cancel_call_count;
  g_task_last_on_cancel_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return 41;
}

extern "C" int objc3_runtime_executor_hop_i32(int value, int executor_tag) {
  ++g_task_executor_hop_call_count;
  g_task_last_executor_hop_executor_tag = executor_tag;
  g_task_last_executor_hop_value = value;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

extern "C" int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag) {
  ++g_actor_isolation_thunk_call_count;
  g_actor_last_isolation_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_enter_nonisolated_i32(int value,
                                                         int executor_tag) {
  ++g_actor_nonisolated_entry_call_count;
  g_actor_last_nonisolated_value = value;
  g_actor_last_nonisolated_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

extern "C" int objc3_runtime_actor_hop_to_executor_i32(int value,
                                                       int executor_tag) {
  ++g_actor_hop_to_executor_call_count;
  g_actor_last_hop_value = value;
  g_actor_last_hop_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_actor_last_hop_result = 0;
    return 0;
  }
  g_actor_last_hop_result = value;
  return value;
}

extern "C" int objc3_runtime_actor_record_replay_proof_i32(int executor_tag) {
  ++g_actor_replay_proof_call_count;
  g_actor_last_replay_proof_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_record_race_guard_i32(int executor_tag) {
  ++g_actor_race_guard_call_count;
  g_actor_last_race_guard_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_bind_executor_i32(int actor_handle,
                                                     int executor_tag) {
  ++g_actor_bind_executor_call_count;
  g_actor_last_bound_actor_handle = actor_handle;
  g_actor_last_bound_executor_tag = executor_tag;
  g_actor_last_mailbox_actor_handle = actor_handle;
  g_actor_last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_actor_last_mailbox_depth = 0;
    return 0;
  }
  g_actor_last_mailbox_depth =
      static_cast<int>(g_actor_mailboxes[actor_handle].size());
  return executor_tag;
}

extern "C" int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle,
                                                       int value,
                                                       int executor_tag) {
  ++g_actor_mailbox_enqueue_call_count;
  g_actor_last_mailbox_actor_handle = actor_handle;
  g_actor_last_mailbox_enqueued_value = value;
  g_actor_last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_actor_last_mailbox_depth = 0;
    return 0;
  }
  std::deque<int> &mailbox = g_actor_mailboxes[actor_handle];
  mailbox.push_back(value);
  g_actor_last_mailbox_depth = static_cast<int>(mailbox.size());
  return value;
}

extern "C" int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                                          int executor_tag) {
  ++g_actor_mailbox_drain_call_count;
  g_actor_last_mailbox_actor_handle = actor_handle;
  g_actor_last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    g_actor_last_mailbox_depth = 0;
    g_actor_last_mailbox_drained_value = 0;
    return 0;
  }
  std::deque<int> &mailbox = g_actor_mailboxes[actor_handle];
  if (mailbox.empty()) {
    g_actor_last_mailbox_depth = 0;
    g_actor_last_mailbox_drained_value = 0;
    return 0;
  }
  const int value = mailbox.front();
  mailbox.pop_front();
  g_actor_last_mailbox_depth = static_cast<int>(mailbox.size());
  g_actor_last_mailbox_drained_value = value;
  return value;
}

extern "C" int objc3_runtime_copy_async_continuation_state_for_testing(
    objc3_runtime_async_continuation_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->allocation_call_count = g_continuation_allocation_call_count;
  snapshot->handoff_call_count = g_continuation_handoff_call_count;
  snapshot->resume_call_count = g_continuation_resume_call_count;
  snapshot->live_continuation_handle_count = g_live_continuation_handles.size();
  snapshot->last_allocated_continuation_handle =
      g_continuation_last_allocated_handle;
  snapshot->last_allocated_resume_entry_tag =
      g_continuation_last_allocated_resume_entry_tag;
  snapshot->last_allocated_executor_tag =
      g_continuation_last_allocated_executor_tag;
  snapshot->last_handoff_continuation_handle =
      g_continuation_last_handoff_handle;
  snapshot->last_handoff_executor_tag = g_continuation_last_handoff_executor_tag;
  snapshot->last_resume_continuation_handle =
      g_continuation_last_resume_handle;
  snapshot->last_resume_result_value = g_continuation_last_resume_result_value;
  snapshot->last_resume_return_value = g_continuation_last_resume_return_value;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->spawn_call_count = g_task_spawn_call_count;
  snapshot->scope_call_count = g_task_scope_call_count;
  snapshot->add_task_call_count = g_task_add_task_call_count;
  snapshot->wait_next_call_count = g_task_wait_next_call_count;
  snapshot->cancel_all_call_count = g_task_cancel_all_call_count;
  snapshot->cancellation_poll_call_count = g_task_cancellation_poll_call_count;
  snapshot->on_cancel_call_count = g_task_on_cancel_call_count;
  snapshot->executor_hop_call_count = g_task_executor_hop_call_count;
  snapshot->last_spawn_kind = g_task_last_spawn_kind;
  snapshot->last_spawn_executor_tag = g_task_last_spawn_executor_tag;
  snapshot->last_scope_executor_tag = g_task_last_scope_executor_tag;
  snapshot->last_add_task_executor_tag = g_task_last_add_task_executor_tag;
  snapshot->last_wait_next_executor_tag = g_task_last_wait_next_executor_tag;
  snapshot->last_cancel_all_executor_tag = g_task_last_cancel_all_executor_tag;
  snapshot->last_cancellation_poll_executor_tag =
      g_task_last_cancellation_poll_executor_tag;
  snapshot->last_on_cancel_executor_tag = g_task_last_on_cancel_executor_tag;
  snapshot->last_executor_hop_executor_tag =
      g_task_last_executor_hop_executor_tag;
  snapshot->last_executor_hop_value = g_task_last_executor_hop_value;
  snapshot->last_wait_next_result = g_task_last_wait_next_result;
  snapshot->last_cancel_all_result = g_task_last_cancel_all_result;
  snapshot->last_cancellation_poll_result =
      g_task_last_cancellation_poll_result;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->isolation_thunk_call_count = g_actor_isolation_thunk_call_count;
  snapshot->nonisolated_entry_call_count =
      g_actor_nonisolated_entry_call_count;
  snapshot->hop_to_executor_call_count = g_actor_hop_to_executor_call_count;
  snapshot->replay_proof_call_count = g_actor_replay_proof_call_count;
  snapshot->race_guard_call_count = g_actor_race_guard_call_count;
  snapshot->bind_executor_call_count = g_actor_bind_executor_call_count;
  snapshot->mailbox_enqueue_call_count = g_actor_mailbox_enqueue_call_count;
  snapshot->mailbox_drain_call_count = g_actor_mailbox_drain_call_count;
  snapshot->last_isolation_executor_tag = g_actor_last_isolation_executor_tag;
  snapshot->last_nonisolated_value = g_actor_last_nonisolated_value;
  snapshot->last_nonisolated_executor_tag =
      g_actor_last_nonisolated_executor_tag;
  snapshot->last_hop_value = g_actor_last_hop_value;
  snapshot->last_hop_executor_tag = g_actor_last_hop_executor_tag;
  snapshot->last_hop_result = g_actor_last_hop_result;
  snapshot->last_replay_proof_executor_tag =
      g_actor_last_replay_proof_executor_tag;
  snapshot->last_race_guard_executor_tag = g_actor_last_race_guard_executor_tag;
  snapshot->last_bound_actor_handle = g_actor_last_bound_actor_handle;
  snapshot->last_bound_executor_tag = g_actor_last_bound_executor_tag;
  snapshot->last_mailbox_actor_handle = g_actor_last_mailbox_actor_handle;
  snapshot->last_mailbox_enqueued_value = g_actor_last_mailbox_enqueued_value;
  snapshot->last_mailbox_executor_tag = g_actor_last_mailbox_executor_tag;
  snapshot->last_mailbox_depth = g_actor_last_mailbox_depth;
  snapshot->last_mailbox_drained_value = g_actor_last_mailbox_drained_value;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
