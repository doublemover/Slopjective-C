#include "runtime/concurrency/actor_state_store.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::RuntimeActorState &state =
      objc3c::runtime::RuntimeActorStateForCurrentThread();
  snapshot->isolation_thunk_call_count = state.isolation_thunk_call_count;
  snapshot->nonisolated_entry_call_count =
      state.nonisolated_entry_call_count;
  snapshot->hop_to_executor_call_count = state.hop_to_executor_call_count;
  snapshot->replay_proof_call_count = state.replay_proof_call_count;
  snapshot->race_guard_call_count = state.race_guard_call_count;
  snapshot->bind_executor_call_count = state.bind_executor_call_count;
  snapshot->mailbox_enqueue_call_count = state.mailbox_enqueue_call_count;
  snapshot->mailbox_drain_call_count = state.mailbox_drain_call_count;
  snapshot->last_isolation_executor_tag = state.last_isolation_executor_tag;
  snapshot->last_nonisolated_value = state.last_nonisolated_value;
  snapshot->last_nonisolated_executor_tag =
      state.last_nonisolated_executor_tag;
  snapshot->last_hop_value = state.last_hop_value;
  snapshot->last_hop_executor_tag = state.last_hop_executor_tag;
  snapshot->last_hop_result = state.last_hop_result;
  snapshot->last_replay_proof_executor_tag =
      state.last_replay_proof_executor_tag;
  snapshot->last_race_guard_executor_tag = state.last_race_guard_executor_tag;
  snapshot->last_bound_actor_handle = state.last_bound_actor_handle;
  snapshot->last_bound_executor_tag = state.last_bound_executor_tag;
  snapshot->last_mailbox_actor_handle = state.last_mailbox_actor_handle;
  snapshot->last_mailbox_enqueued_value = state.last_mailbox_enqueued_value;
  snapshot->last_mailbox_executor_tag = state.last_mailbox_executor_tag;
  snapshot->last_mailbox_depth = state.last_mailbox_depth;
  snapshot->last_mailbox_drained_value = state.last_mailbox_drained_value;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
