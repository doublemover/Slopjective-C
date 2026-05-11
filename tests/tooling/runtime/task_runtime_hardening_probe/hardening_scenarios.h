#pragma once

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

struct TaskRuntimeHardeningScenario {
  int group_spawn_kind;
  int group_executor_tag;
  int scope_group_tag;
  int group_task_tag;
  int cancellation_task_tag;
  int wait_group_tag;
  int executor_hop_tag;
  int cancel_group_tag;
  int on_cancel_task_tag;
  int detached_spawn_kind;
  int detached_executor_tag;
};

constexpr TaskRuntimeHardeningScenario kReplayScenario = {
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3};

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
