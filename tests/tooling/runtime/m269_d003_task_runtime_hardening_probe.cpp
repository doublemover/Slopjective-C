#include <iostream>

#include "runtime/objc3_runtime_bootstrap_internal.h"

struct PassResult {
  int spawn_group = 0;
  int scope = 0;
  int add_task = 0;
  int cancelled = 0;
  int wait_next = 0;
  int hop = 0;
  int cancel_all = 0;
  int on_cancel = 0;
  int spawn_detached = 0;
  int copy_task_status = 0;
  int copy_memory_status = 0;
  int copy_arc_status = 0;
  objc3_runtime_task_runtime_state_snapshot task{};
  objc3_runtime_memory_management_state_snapshot memory{};
  objc3_runtime_arc_debug_state_snapshot arc{};
};

PassResult RunPass() {
  PassResult result{};
  objc3_runtime_reset_for_testing();
  objc3_runtime_push_autoreleasepool_scope();
  result.spawn_group = objc3_runtime_spawn_task_i32(1, 2);
  result.scope = objc3_runtime_enter_task_group_scope_i32(2);
  result.add_task = objc3_runtime_add_task_group_task_i32(2);
  result.cancelled = objc3_runtime_task_is_cancelled_i32(2);
  result.wait_next = objc3_runtime_wait_task_group_next_i32(2);
  result.hop = objc3_runtime_executor_hop_i32(result.wait_next, 2);
  result.cancel_all = objc3_runtime_cancel_task_group_i32(2);
  result.on_cancel = objc3_runtime_task_on_cancel_i32(2);
  result.spawn_detached = objc3_runtime_spawn_task_i32(2, 3);
  objc3_runtime_pop_autoreleasepool_scope();
  result.copy_task_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&result.task);
  result.copy_memory_status =
      objc3_runtime_copy_memory_management_state_for_testing(&result.memory);
  result.copy_arc_status =
      objc3_runtime_copy_arc_debug_state_for_testing(&result.arc);
  return result;
}

bool Equivalent(const PassResult &lhs, const PassResult &rhs) {
  return lhs.spawn_group == rhs.spawn_group && lhs.scope == rhs.scope &&
         lhs.add_task == rhs.add_task && lhs.cancelled == rhs.cancelled &&
         lhs.wait_next == rhs.wait_next && lhs.hop == rhs.hop &&
         lhs.cancel_all == rhs.cancel_all && lhs.on_cancel == rhs.on_cancel &&
         lhs.spawn_detached == rhs.spawn_detached &&
         lhs.copy_task_status == rhs.copy_task_status &&
         lhs.copy_memory_status == rhs.copy_memory_status &&
         lhs.copy_arc_status == rhs.copy_arc_status &&
         lhs.task.spawn_call_count == rhs.task.spawn_call_count &&
         lhs.task.scope_call_count == rhs.task.scope_call_count &&
         lhs.task.add_task_call_count == rhs.task.add_task_call_count &&
         lhs.task.wait_next_call_count == rhs.task.wait_next_call_count &&
         lhs.task.cancel_all_call_count == rhs.task.cancel_all_call_count &&
         lhs.task.cancellation_poll_call_count == rhs.task.cancellation_poll_call_count &&
         lhs.task.on_cancel_call_count == rhs.task.on_cancel_call_count &&
         lhs.task.executor_hop_call_count == rhs.task.executor_hop_call_count &&
         lhs.task.last_spawn_kind == rhs.task.last_spawn_kind &&
         lhs.task.last_spawn_executor_tag == rhs.task.last_spawn_executor_tag &&
         lhs.task.last_wait_next_result == rhs.task.last_wait_next_result &&
         lhs.task.last_executor_hop_executor_tag == rhs.task.last_executor_hop_executor_tag &&
         lhs.task.last_executor_hop_value == rhs.task.last_executor_hop_value &&
         lhs.memory.autoreleasepool_depth == rhs.memory.autoreleasepool_depth &&
         lhs.memory.autoreleasepool_max_depth == rhs.memory.autoreleasepool_max_depth &&
         lhs.arc.autoreleasepool_push_count == rhs.arc.autoreleasepool_push_count &&
         lhs.arc.autoreleasepool_pop_count == rhs.arc.autoreleasepool_pop_count;
}

int main() {
  const PassResult pass1 = RunPass();
  const PassResult pass2 = RunPass();

  std::cout << "pass1_copy_task_status=" << pass1.copy_task_status << "\n";
  std::cout << "pass1_copy_memory_status=" << pass1.copy_memory_status << "\n";
  std::cout << "pass1_copy_arc_status=" << pass1.copy_arc_status << "\n";
  std::cout << "pass1_spawn_group=" << pass1.spawn_group << "\n";
  std::cout << "pass1_wait_next=" << pass1.wait_next << "\n";
  std::cout << "pass1_cancel_all=" << pass1.cancel_all << "\n";
  std::cout << "pass1_spawn_call_count=" << pass1.task.spawn_call_count << "\n";
  std::cout << "pass1_cancel_all_call_count=" << pass1.task.cancel_all_call_count << "\n";
  std::cout << "pass1_executor_hop_call_count=" << pass1.task.executor_hop_call_count << "\n";
  std::cout << "pass1_last_executor_hop_value=" << pass1.task.last_executor_hop_value << "\n";
  std::cout << "pass1_autoreleasepool_depth=" << pass1.memory.autoreleasepool_depth << "\n";
  std::cout << "pass1_autoreleasepool_max_depth=" << pass1.memory.autoreleasepool_max_depth << "\n";
  std::cout << "pass1_autoreleasepool_push_count=" << pass1.arc.autoreleasepool_push_count << "\n";
  std::cout << "pass1_autoreleasepool_pop_count=" << pass1.arc.autoreleasepool_pop_count << "\n";
  std::cout << "replay_equal=" << (Equivalent(pass1, pass2) ? 1 : 0) << "\n";

  const bool ok =
      pass1.copy_task_status == 0 && pass1.copy_memory_status == 0 &&
      pass1.copy_arc_status == 0 && pass1.spawn_group == 111 &&
      pass1.scope == 1 && pass1.add_task == 1 && pass1.cancelled == 0 &&
      pass1.wait_next == 23 && pass1.hop == 23 && pass1.cancel_all == 31 &&
      pass1.on_cancel == 41 && pass1.spawn_detached == 121 &&
      pass1.task.spawn_call_count == 2 && pass1.task.scope_call_count == 1 &&
      pass1.task.add_task_call_count == 1 &&
      pass1.task.wait_next_call_count == 1 &&
      pass1.task.cancel_all_call_count == 1 &&
      pass1.task.cancellation_poll_call_count == 1 &&
      pass1.task.on_cancel_call_count == 1 &&
      pass1.task.executor_hop_call_count == 1 &&
      pass1.task.last_spawn_kind == 2 &&
      pass1.task.last_spawn_executor_tag == 3 &&
      pass1.task.last_wait_next_result == 23 &&
      pass1.task.last_executor_hop_executor_tag == 2 &&
      pass1.task.last_executor_hop_value == 23 &&
      pass1.memory.autoreleasepool_depth == 0 &&
      pass1.memory.autoreleasepool_max_depth == 1 &&
      pass1.arc.autoreleasepool_push_count == 1 &&
      pass1.arc.autoreleasepool_pop_count == 1 && Equivalent(pass1, pass2);
  return ok ? 0 : 1;
}
