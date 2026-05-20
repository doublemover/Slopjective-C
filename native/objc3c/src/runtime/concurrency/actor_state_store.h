#pragma once

#include <cstdint>
#include <deque>
#include <unordered_map>

#include "runtime/concurrency/runtime_concurrency_snapshot_contracts.h"

namespace objc3c::runtime {

struct RuntimeActorState {
  std::uint64_t isolation_thunk_call_count = 0;
  std::uint64_t nonisolated_entry_call_count = 0;
  std::uint64_t hop_to_executor_call_count = 0;
  std::uint64_t replay_proof_call_count = 0;
  std::uint64_t race_guard_call_count = 0;
  std::uint64_t bind_executor_call_count = 0;
  std::uint64_t mailbox_enqueue_call_count = 0;
  std::uint64_t mailbox_drain_call_count = 0;
  std::uint64_t failed_operation_count = 0;
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
  int last_operation_succeeded = 1;
  int last_failure_code = OBJC3_RUNTIME_ACTOR_FAILURE_NONE;
  std::unordered_map<int, std::deque<int>> mailboxes;
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
