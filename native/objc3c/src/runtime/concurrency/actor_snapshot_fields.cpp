#include "runtime/concurrency/actor_snapshot_fields.h"

#include "runtime/concurrency/actor_state_store.h"

namespace objc3c::runtime {

void ResetRuntimeActorRuntimeStateSnapshot(
    objc3_runtime_actor_runtime_state_snapshot &snapshot) {
  snapshot.isolation_thunk_call_count = 0;
  snapshot.nonisolated_entry_call_count = 0;
  snapshot.hop_to_executor_call_count = 0;
  snapshot.replay_proof_call_count = 0;
  snapshot.race_guard_call_count = 0;
  snapshot.bind_executor_call_count = 0;
  snapshot.mailbox_enqueue_call_count = 0;
  snapshot.mailbox_drain_call_count = 0;
  snapshot.failed_operation_count = 0;
  snapshot.last_isolation_executor_tag = 0;
  snapshot.last_nonisolated_value = 0;
  snapshot.last_nonisolated_executor_tag = 0;
  snapshot.last_hop_value = 0;
  snapshot.last_hop_executor_tag = 0;
  snapshot.last_hop_result = 0;
  snapshot.last_replay_proof_executor_tag = 0;
  snapshot.last_race_guard_executor_tag = 0;
  snapshot.last_bound_actor_handle = 0;
  snapshot.last_bound_executor_tag = 0;
  snapshot.last_mailbox_actor_handle = 0;
  snapshot.last_mailbox_enqueued_value = 0;
  snapshot.last_mailbox_executor_tag = 0;
  snapshot.last_mailbox_depth = 0;
  snapshot.last_mailbox_drained_value = 0;
  snapshot.last_operation_succeeded = 1;
  snapshot.last_failure_code = OBJC3_RUNTIME_ACTOR_FAILURE_NONE;
}

void PopulateRuntimeActorRuntimeStateSnapshot(
    const RuntimeActorState &state,
    objc3_runtime_actor_runtime_state_snapshot &snapshot) {
  snapshot.isolation_thunk_call_count = state.isolation_thunk_call_count;
  snapshot.nonisolated_entry_call_count =
      state.nonisolated_entry_call_count;
  snapshot.hop_to_executor_call_count = state.hop_to_executor_call_count;
  snapshot.replay_proof_call_count = state.replay_proof_call_count;
  snapshot.race_guard_call_count = state.race_guard_call_count;
  snapshot.bind_executor_call_count = state.bind_executor_call_count;
  snapshot.mailbox_enqueue_call_count = state.mailbox_enqueue_call_count;
  snapshot.mailbox_drain_call_count = state.mailbox_drain_call_count;
  snapshot.failed_operation_count = state.failed_operation_count;
  snapshot.last_isolation_executor_tag = state.last_isolation_executor_tag;
  snapshot.last_nonisolated_value = state.last_nonisolated_value;
  snapshot.last_nonisolated_executor_tag =
      state.last_nonisolated_executor_tag;
  snapshot.last_hop_value = state.last_hop_value;
  snapshot.last_hop_executor_tag = state.last_hop_executor_tag;
  snapshot.last_hop_result = state.last_hop_result;
  snapshot.last_replay_proof_executor_tag =
      state.last_replay_proof_executor_tag;
  snapshot.last_race_guard_executor_tag = state.last_race_guard_executor_tag;
  snapshot.last_bound_actor_handle = state.last_bound_actor_handle;
  snapshot.last_bound_executor_tag = state.last_bound_executor_tag;
  snapshot.last_mailbox_actor_handle = state.last_mailbox_actor_handle;
  snapshot.last_mailbox_enqueued_value = state.last_mailbox_enqueued_value;
  snapshot.last_mailbox_executor_tag = state.last_mailbox_executor_tag;
  snapshot.last_mailbox_depth = state.last_mailbox_depth;
  snapshot.last_mailbox_drained_value = state.last_mailbox_drained_value;
  snapshot.last_operation_succeeded = state.last_operation_succeeded;
  snapshot.last_failure_code = state.last_failure_code;
}

}  // namespace objc3c::runtime
