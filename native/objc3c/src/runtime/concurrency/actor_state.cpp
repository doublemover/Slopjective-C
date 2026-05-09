#include "runtime/concurrency/actor_state.h"

#include "runtime/concurrency/actor.h"
#include "runtime/concurrency/executor.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <deque>
#include <unordered_map>

namespace objc3c::runtime {
namespace detail {

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

RuntimeActorState &ActorStateForCurrentThread() {
  thread_local RuntimeActorState state;
  return state;
}

}  // namespace detail

void ResetRuntimeActorStateForTesting() {
  detail::ActorStateForCurrentThread() = detail::RuntimeActorState{};
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.isolation_thunk_call_count;
  state.last_isolation_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_enter_nonisolated_i32(int value,
                                                         int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.nonisolated_entry_call_count;
  state.last_nonisolated_value = value;
  state.last_nonisolated_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return value;
}

extern "C" int objc3_runtime_actor_hop_to_executor_i32(int value,
                                                       int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.hop_to_executor_call_count;
  state.last_hop_value = value;
  state.last_hop_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_hop_result = 0;
    return 0;
  }
  state.last_hop_result = value;
  return value;
}

extern "C" int objc3_runtime_actor_record_replay_proof_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.replay_proof_call_count;
  state.last_replay_proof_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_record_race_guard_i32(int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.race_guard_call_count;
  state.last_race_guard_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    return 0;
  }
  return executor_tag;
}

extern "C" int objc3_runtime_actor_bind_executor_i32(int actor_handle,
                                                     int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.bind_executor_call_count;
  state.last_bound_actor_handle = actor_handle;
  state.last_bound_executor_tag = executor_tag;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    return 0;
  }
  state.last_mailbox_depth =
      static_cast<int>(state.mailboxes[actor_handle].size());
  return executor_tag;
}

extern "C" int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle,
                                                       int value,
                                                       int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.mailbox_enqueue_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_enqueued_value = value;
  state.last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  mailbox.push_back(value);
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  return value;
}

extern "C" int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                                          int executor_tag) {
  objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
  ++state.mailbox_drain_call_count;
  state.last_mailbox_actor_handle = actor_handle;
  state.last_mailbox_executor_tag = executor_tag;
  if (!objc3c::runtime::RuntimeActorHandleIsValid(actor_handle) ||
      !objc3c::runtime::RuntimeExecutorTagIsValid(executor_tag)) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    return 0;
  }
  std::deque<int> &mailbox = state.mailboxes[actor_handle];
  if (mailbox.empty()) {
    state.last_mailbox_depth = 0;
    state.last_mailbox_drained_value = 0;
    return 0;
  }
  const int value = mailbox.front();
  mailbox.pop_front();
  state.last_mailbox_depth = static_cast<int>(mailbox.size());
  state.last_mailbox_drained_value = value;
  return value;
}

extern "C" int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const objc3c::runtime::detail::RuntimeActorState &state =
      objc3c::runtime::detail::ActorStateForCurrentThread();
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
