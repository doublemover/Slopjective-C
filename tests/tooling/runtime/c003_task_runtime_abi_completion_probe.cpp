#include <cstdint>
#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  const int spawn_group = objc3_runtime_spawn_task_i32(1, 2);
  const int scope = objc3_runtime_enter_task_group_scope_i32(2);
  const int add_task = objc3_runtime_add_task_group_task_i32(2);
  const int cancelled = objc3_runtime_task_is_cancelled_i32(2);
  const int wait_next = objc3_runtime_wait_task_group_next_i32(2);
  const int hop = objc3_runtime_executor_hop_i32(wait_next, 2);
  const int cancel_all = objc3_runtime_cancel_task_group_i32(2);
  const int on_cancel = objc3_runtime_task_on_cancel_i32(2);
  const int spawn_detached = objc3_runtime_spawn_task_i32(2, 3);

  objc3_runtime_task_runtime_state_snapshot snapshot{};
  const int copy_status = objc3_runtime_copy_task_runtime_state_for_testing(&snapshot);

  std::cout << "spawn_group=" << spawn_group << "\n";
  std::cout << "scope=" << scope << "\n";
  std::cout << "add_task=" << add_task << "\n";
  std::cout << "cancelled=" << cancelled << "\n";
  std::cout << "wait_next=" << wait_next << "\n";
  std::cout << "hop=" << hop << "\n";
  std::cout << "cancel_all=" << cancel_all << "\n";
  std::cout << "on_cancel=" << on_cancel << "\n";
  std::cout << "spawn_detached=" << spawn_detached << "\n";
  std::cout << "copy_status=" << copy_status << "\n";
  std::cout << "spawn_call_count=" << snapshot.spawn_call_count << "\n";
  std::cout << "scope_call_count=" << snapshot.scope_call_count << "\n";
  std::cout << "add_task_call_count=" << snapshot.add_task_call_count << "\n";
  std::cout << "wait_next_call_count=" << snapshot.wait_next_call_count << "\n";
  std::cout << "cancel_all_call_count=" << snapshot.cancel_all_call_count << "\n";
  std::cout << "cancellation_poll_call_count=" << snapshot.cancellation_poll_call_count << "\n";
  std::cout << "on_cancel_call_count=" << snapshot.on_cancel_call_count << "\n";
  std::cout << "executor_hop_call_count=" << snapshot.executor_hop_call_count << "\n";
  std::cout << "last_spawn_kind=" << snapshot.last_spawn_kind << "\n";
  std::cout << "last_spawn_executor_tag=" << snapshot.last_spawn_executor_tag << "\n";
  std::cout << "last_wait_next_result=" << snapshot.last_wait_next_result << "\n";
  std::cout << "last_executor_hop_executor_tag=" << snapshot.last_executor_hop_executor_tag << "\n";
  std::cout << "last_executor_hop_value=" << snapshot.last_executor_hop_value << "\n";

  return (copy_status == 0 && spawn_group == 111 && scope == 1 && add_task == 1 &&
          cancelled == 0 && wait_next == 23 && hop == 23 && cancel_all == 31 &&
          on_cancel == 41 && spawn_detached == 121 && snapshot.spawn_call_count == 2 &&
          snapshot.scope_call_count == 1 && snapshot.add_task_call_count == 1 &&
          snapshot.wait_next_call_count == 1 && snapshot.cancel_all_call_count == 1 &&
          snapshot.cancellation_poll_call_count == 1 && snapshot.on_cancel_call_count == 1 &&
          snapshot.executor_hop_call_count == 1 && snapshot.last_spawn_kind == 2 &&
          snapshot.last_spawn_executor_tag == 3 && snapshot.last_wait_next_result == 23 &&
          snapshot.last_executor_hop_executor_tag == 2 && snapshot.last_executor_hop_value == 23)
             ? 0
             : 1;
}
