#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_ownership_contract.h"

#include <iostream>

namespace {

int Fail(const char *message) {
  std::cerr << "stdlib-concurrency-runtime-probe: " << message << "\n";
  return 1;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  const int spawn = objc3_runtime_spawn_task_i32(1, 4);
  const int child_spawn = objc3_runtime_spawn_task_i32(1, 4);
  const int detached_spawn = objc3_runtime_spawn_task_i32(2, 4);
  const int scope = objc3_runtime_enter_task_group_scope_i32(4);
  const int add_first = objc3_runtime_add_task_group_task_i32(4);
  const int add_second = objc3_runtime_add_task_group_task_i32(4);
  const int cancellation_query = objc3_runtime_task_is_cancelled_i32(4);
  const int wait_first = objc3_runtime_wait_task_group_next_i32(4);
  const int join_status = objc3_runtime_executor_hop_i32(wait_first, 4);
  const int wait_second = objc3_runtime_wait_task_group_next_i32(4);
  const int cancel_checkpoint = objc3_runtime_cancel_task_group_i32(4);
  const int cancelled_join = objc3_runtime_task_on_cancel_i32(4);

  objc3_runtime_task_runtime_state_snapshot snapshot{};
  const int copy_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&snapshot);

  if (spawn != 111 || child_spawn != 111 || detached_spawn != 121) {
    return Fail("spawn token helpers did not route through runtime task spawn");
  }
  if (scope != 1 || add_first != 1 || add_second != 1) {
    return Fail("task group helper sequence did not enter/add through runtime");
  }
  if (cancellation_query != 0) {
    return Fail("cancellation query did not observe uncancelled group");
  }
  if (wait_first != 25 || join_status != 25 || wait_second != 26) {
    return Fail("join/wait helpers did not preserve deterministic FIFO order");
  }
  if (cancel_checkpoint != 31 || cancelled_join != 41) {
    return Fail("cancellation checkpoint helpers did not route through runtime");
  }
  if (copy_status != 0) {
    return Fail("task runtime snapshot copy failed");
  }
  if (snapshot.spawn_call_count != 3 || snapshot.scope_call_count != 1 ||
      snapshot.add_task_call_count != 2 || snapshot.wait_next_call_count != 2 ||
      snapshot.cancel_all_call_count != 1 ||
      snapshot.cancellation_poll_call_count != 1 ||
      snapshot.on_cancel_call_count != 1 ||
      snapshot.executor_hop_call_count != 1) {
    return Fail("task runtime call counters drifted");
  }
  if (snapshot.scheduler_enqueue_count != 3 ||
      snapshot.scheduler_dequeue_count != 2 ||
      snapshot.scheduler_cancelled_count != 0 ||
      snapshot.scheduler_sequence != 5 ||
      snapshot.deadlock_guard_passed != 1 ||
      snapshot.race_guard_passed != 1) {
    return Fail("deterministic scheduler state drifted");
  }
  if (snapshot.active_group_executor_tag != 4 ||
      snapshot.pending_group_task_count != 0 ||
      snapshot.completed_group_task_count != 2 ||
      snapshot.cancelled_group_task_count != 0 ||
      snapshot.group_cancelled != 1 ||
      snapshot.cancellation_generation != 1 ||
      snapshot.last_queue_drain_result != 26 ||
      snapshot.last_dequeued_task_handle != 26 ||
      snapshot.last_dequeued_executor_tag != 4) {
    return Fail("task group/cancellation snapshot drifted");
  }

  std::cout << "{"
            << "\"spawn_call_count\":" << snapshot.spawn_call_count
            << ",\"scope_call_count\":" << snapshot.scope_call_count
            << ",\"add_task_call_count\":" << snapshot.add_task_call_count
            << ",\"wait_next_call_count\":" << snapshot.wait_next_call_count
            << ",\"cancel_all_call_count\":"
            << snapshot.cancel_all_call_count
            << ",\"executor_hop_call_count\":"
            << snapshot.executor_hop_call_count
            << ",\"scheduler_enqueue_count\":"
            << snapshot.scheduler_enqueue_count
            << ",\"scheduler_dequeue_count\":"
            << snapshot.scheduler_dequeue_count
            << ",\"scheduler_sequence\":" << snapshot.scheduler_sequence
            << ",\"last_queue_drain_result\":"
            << snapshot.last_queue_drain_result << "}\n";
  return 0;
}
