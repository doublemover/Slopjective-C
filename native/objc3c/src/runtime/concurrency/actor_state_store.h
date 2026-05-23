#pragma once

#include <cstdint>
#include <deque>
#include <unordered_map>

#include "runtime/concurrency/runtime_concurrency_snapshot_contracts.h"

namespace objc3c::runtime {

struct RuntimeActorMailboxMessage {
  std::uint64_t message_id = 0;
  int value = 0;
  int executor_tag = 0;
};

struct RuntimeActorState {
  std::uint64_t isolation_thunk_call_count = 0;
  std::uint64_t nonisolated_entry_call_count = 0;
  std::uint64_t hop_to_executor_call_count = 0;
  std::uint64_t replay_proof_call_count = 0;
  std::uint64_t race_guard_call_count = 0;
  std::uint64_t bind_executor_call_count = 0;
  std::uint64_t mailbox_enqueue_call_count = 0;
  std::uint64_t mailbox_drain_call_count = 0;
  std::uint64_t mailbox_cancel_call_count = 0;
  std::uint64_t mailbox_error_call_count = 0;
  std::uint64_t mailbox_shutdown_call_count = 0;
  std::uint64_t failed_operation_count = 0;
  std::uint64_t actor_executor_binding_count = 0;
  std::uint64_t mailbox_message_sequence = 0;
  std::uint64_t last_mailbox_message_id = 0;
  std::uint64_t last_mailbox_enqueue_sequence = 0;
  std::uint64_t last_mailbox_dequeue_sequence = 0;
  std::uint64_t last_mailbox_completion_sequence = 0;
  std::uint64_t last_mailbox_cancelled_count = 0;
  std::uint64_t last_mailbox_shutdown_pending_count = 0;
  int last_isolation_executor_tag = 0;
  int last_nonisolated_value = 0;
  int last_nonisolated_executor_tag = 0;
  int last_hop_value = 0;
  int last_hop_executor_tag = 0;
  int last_hop_result = 0;
  int last_replay_proof_executor_tag = 0;
  int last_race_guard_executor_tag = 0;
  int last_bound_actor_handle = 0;
  int last_bound_executor_tag = 0;
  int last_mailbox_actor_handle = 0;
  int last_mailbox_enqueued_value = 0;
  int last_mailbox_executor_tag = 0;
  int last_mailbox_depth = 0;
  int last_mailbox_drained_value = 0;
  int last_expected_executor_tag = 0;
  int last_mailbox_error_code = 0;
  int mailbox_identity_guard_passed = 1;
  int executor_binding_guard_passed = 1;
  int mailbox_ordering_guard_passed = 1;
  int mailbox_shutdown_guard_passed = 1;
  int last_operation_succeeded = 1;
  int last_failure_code = OBJC3_RUNTIME_ACTOR_FAILURE_NONE;
  std::unordered_map<int, int> actor_executor_bindings;
  std::unordered_map<int, std::deque<RuntimeActorMailboxMessage>> mailboxes;
  std::unordered_map<int, int> actor_mailbox_shutdowns;
  std::unordered_map<int, std::uint64_t> actor_last_completed_sequence;
};

inline void RecordRuntimeActorSuccess(RuntimeActorState &state) {
  state.last_operation_succeeded = 1;
  state.last_failure_code = OBJC3_RUNTIME_ACTOR_FAILURE_NONE;
}

inline void RecordRuntimeActorFailure(RuntimeActorState &state,
                                      int failure_code) {
  ++state.failed_operation_count;
  state.last_operation_succeeded = 0;
  state.last_failure_code = failure_code;
}

RuntimeActorState &RuntimeActorStateForCurrentThread();

}  // namespace objc3c::runtime
