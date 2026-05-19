#pragma once

#include "hardening_scenarios.h"
#include "probe_state.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline void ResetTaskRuntimeFixture() {
  objc3_runtime_reset_for_testing();
}

inline PassResult RunPass(const TaskRuntimeHardeningScenario &scenario) {
  PassResult result{};
  ResetTaskRuntimeFixture();
  objc3_runtime_push_autoreleasepool_scope();
  result.spawn_group = objc3_runtime_spawn_task_i32(
      scenario.group_spawn_kind, scenario.group_executor_tag);
  result.scope =
      objc3_runtime_enter_task_group_scope_i32(scenario.scope_group_tag);
  result.add_task =
      objc3_runtime_add_task_group_task_i32(scenario.group_task_tag);
  result.cancelled =
      objc3_runtime_task_is_cancelled_i32(scenario.cancellation_task_tag);
  result.wait_next =
      objc3_runtime_wait_task_group_next_i32(scenario.wait_group_tag);
  result.hop =
      objc3_runtime_executor_hop_i32(result.wait_next, scenario.executor_hop_tag);
  result.cancel_all =
      objc3_runtime_cancel_task_group_i32(scenario.cancel_group_tag);
  result.on_cancel =
      objc3_runtime_task_on_cancel_i32(scenario.on_cancel_task_tag);
  result.spawn_detached = objc3_runtime_spawn_task_i32(
      scenario.detached_spawn_kind, scenario.detached_executor_tag);
  objc3_runtime_pop_autoreleasepool_scope();
  result.copy_task_status =
      objc3_runtime_copy_task_runtime_state_for_testing(&result.task);
  result.copy_memory_status =
      objc3_runtime_copy_memory_management_state_for_testing(&result.memory);
  result.copy_arc_status =
      objc3_runtime_copy_arc_debug_state_for_testing(&result.arc);
  return result;
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
