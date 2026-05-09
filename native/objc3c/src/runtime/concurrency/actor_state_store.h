#pragma once

#include <cstdint>
#include <deque>
#include <unordered_map>

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
  std::unordered_map<int, std::deque<int>> mailboxes;
};

RuntimeActorState &RuntimeActorStateForCurrentThread();

}  // namespace objc3c::runtime
