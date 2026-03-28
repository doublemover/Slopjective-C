#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_reset_for_testing();

  const int replay = objc3_runtime_actor_record_replay_proof_i32(1);
  const int guard = objc3_runtime_actor_record_race_guard_i32(1);
  const int isolation = objc3_runtime_actor_enter_isolation_thunk_i32(1);
  const int bound = objc3_runtime_actor_bind_executor_i32(41, isolation);
  const int enqueued = objc3_runtime_actor_mailbox_enqueue_i32(41, 23, bound);
  const int drained = objc3_runtime_actor_mailbox_drain_next_i32(41, bound);

  objc3_runtime_actor_runtime_state_snapshot snapshot{};
  const int copy_status = objc3_runtime_copy_actor_runtime_state_for_testing(&snapshot);

  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "replay=" << replay << "\n";
  std::cout << "guard=" << guard << "\n";
  std::cout << "isolation=" << isolation << "\n";
  std::cout << "bound=" << bound << "\n";
  std::cout << "enqueued=" << enqueued << "\n";
  std::cout << "drained=" << drained << "\n";
  std::cout << "replay_proof_call_count=" << snapshot.replay_proof_call_count << "\n";
  std::cout << "race_guard_call_count=" << snapshot.race_guard_call_count << "\n";
  std::cout << "isolation_thunk_call_count=" << snapshot.isolation_thunk_call_count << "\n";
  std::cout << "bind_executor_call_count=" << snapshot.bind_executor_call_count << "\n";
  std::cout << "mailbox_enqueue_call_count=" << snapshot.mailbox_enqueue_call_count << "\n";
  std::cout << "mailbox_drain_call_count=" << snapshot.mailbox_drain_call_count << "\n";
  std::cout << "last_replay_proof_executor_tag=" << snapshot.last_replay_proof_executor_tag << "\n";
  std::cout << "last_race_guard_executor_tag=" << snapshot.last_race_guard_executor_tag << "\n";
  std::cout << "last_isolation_executor_tag=" << snapshot.last_isolation_executor_tag << "\n";
  std::cout << "last_bound_actor_handle=" << snapshot.last_bound_actor_handle << "\n";
  std::cout << "last_bound_executor_tag=" << snapshot.last_bound_executor_tag << "\n";
  std::cout << "last_mailbox_actor_handle=" << snapshot.last_mailbox_actor_handle << "\n";
  std::cout << "last_mailbox_enqueued_value=" << snapshot.last_mailbox_enqueued_value << "\n";
  std::cout << "last_mailbox_executor_tag=" << snapshot.last_mailbox_executor_tag << "\n";
  std::cout << "last_mailbox_depth=" << snapshot.last_mailbox_depth << "\n";
  std::cout << "last_mailbox_drained_value=" << snapshot.last_mailbox_drained_value << "\n";

  return (copy_status == 0 && replay == 1 && guard == 1 && isolation == 1 &&
          bound == 1 && enqueued == 23 && drained == 23 &&
          snapshot.replay_proof_call_count == 1 &&
          snapshot.race_guard_call_count == 1 &&
          snapshot.isolation_thunk_call_count == 1 &&
          snapshot.bind_executor_call_count == 1 &&
          snapshot.mailbox_enqueue_call_count == 1 &&
          snapshot.mailbox_drain_call_count == 1 &&
          snapshot.last_replay_proof_executor_tag == 1 &&
          snapshot.last_race_guard_executor_tag == 1 &&
          snapshot.last_isolation_executor_tag == 1 &&
          snapshot.last_bound_actor_handle == 41 &&
          snapshot.last_bound_executor_tag == 1 &&
          snapshot.last_mailbox_actor_handle == 41 &&
          snapshot.last_mailbox_enqueued_value == 23 &&
          snapshot.last_mailbox_executor_tag == 1 &&
          snapshot.last_mailbox_depth == 0 &&
          snapshot.last_mailbox_drained_value == 23)
             ? 0
             : 1;
}

