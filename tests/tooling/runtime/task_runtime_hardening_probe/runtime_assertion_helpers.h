#pragma once

#include "probe_state.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline bool Equivalent(const PassResult &lhs, const PassResult &rhs) {
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
         lhs.task.cancellation_poll_call_count ==
             rhs.task.cancellation_poll_call_count &&
         lhs.task.on_cancel_call_count == rhs.task.on_cancel_call_count &&
         lhs.task.executor_hop_call_count == rhs.task.executor_hop_call_count &&
         lhs.task.last_spawn_kind == rhs.task.last_spawn_kind &&
         lhs.task.last_spawn_executor_tag == rhs.task.last_spawn_executor_tag &&
         lhs.task.last_wait_next_result == rhs.task.last_wait_next_result &&
         lhs.task.last_executor_hop_executor_tag ==
             rhs.task.last_executor_hop_executor_tag &&
         lhs.task.last_executor_hop_value ==
             rhs.task.last_executor_hop_value &&
         lhs.memory.autoreleasepool_depth ==
             rhs.memory.autoreleasepool_depth &&
         lhs.memory.autoreleasepool_max_depth ==
             rhs.memory.autoreleasepool_max_depth &&
         lhs.arc.autoreleasepool_push_count ==
             rhs.arc.autoreleasepool_push_count &&
         lhs.arc.autoreleasepool_pop_count ==
             rhs.arc.autoreleasepool_pop_count;
}

inline bool SnapshotCopiesSucceeded(const PassResult &pass) {
  return pass.copy_task_status == 0 && pass.copy_memory_status == 0 &&
         pass.copy_arc_status == 0;
}

inline bool TaskRuntimeCallsReturnExpectedValues(const PassResult &pass) {
  return pass.spawn_group == 111 && pass.scope == 1 && pass.add_task == 1 &&
         pass.cancelled == 0 && pass.wait_next == 23 && pass.hop == 23 &&
         pass.cancel_all == 31 && pass.on_cancel == 41 &&
         pass.spawn_detached == 121;
}

inline bool TaskRuntimeCountersMatchExpectedValues(const PassResult &pass) {
  return pass.task.spawn_call_count == 2 && pass.task.scope_call_count == 1 &&
         pass.task.add_task_call_count == 1 &&
         pass.task.wait_next_call_count == 1 &&
         pass.task.cancel_all_call_count == 1 &&
         pass.task.cancellation_poll_call_count == 1 &&
         pass.task.on_cancel_call_count == 1 &&
         pass.task.executor_hop_call_count == 1 &&
         pass.task.last_spawn_kind == 2 &&
         pass.task.last_spawn_executor_tag == 3 &&
         pass.task.last_wait_next_result == 23 &&
         pass.task.last_executor_hop_executor_tag == 2 &&
         pass.task.last_executor_hop_value == 23;
}

inline bool MemoryAndArcHardeningCountersMatchExpectedValues(
    const PassResult &pass) {
  return pass.memory.autoreleasepool_depth == 0 &&
         pass.memory.autoreleasepool_max_depth == 1 &&
         pass.arc.autoreleasepool_push_count == 1 &&
         pass.arc.autoreleasepool_pop_count == 1;
}

inline bool PassAssertionsPassed(const PassResult &pass) {
  return SnapshotCopiesSucceeded(pass) &&
         TaskRuntimeCallsReturnExpectedValues(pass) &&
         TaskRuntimeCountersMatchExpectedValues(pass) &&
         MemoryAndArcHardeningCountersMatchExpectedValues(pass);
}

inline bool ProbeAssertionsPassed(const ProbeRun &run) {
  return PassAssertionsPassed(run.pass1) && Equivalent(run.pass1, run.pass2);
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
