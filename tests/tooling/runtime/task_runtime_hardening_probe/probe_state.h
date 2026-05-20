#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

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
  int after_add_copy_status = 0;
  int after_wait_copy_status = 0;
  int copy_task_status = 0;
  int copy_memory_status = 0;
  int copy_arc_status = 0;
  objc3_runtime_task_runtime_state_snapshot after_add_task{};
  objc3_runtime_task_runtime_state_snapshot after_wait_next{};
  objc3_runtime_task_runtime_state_snapshot task{};
  objc3_runtime_memory_management_state_snapshot memory{};
  objc3_runtime_arc_debug_state_snapshot arc{};
};

struct InvalidHandleResult {
  int invalid_spawn_kind = 0;
  int invalid_spawn_executor = 0;
  int missing_group_add = 0;
  int missing_group_wait = 0;
  int missing_group_cancel = 0;
  int scope = 0;
  int executor_mismatch_add = 0;
  int copy_task_status = 0;
  objc3_runtime_task_runtime_state_snapshot task{};
};

struct ProbeRun {
  PassResult pass1;
  PassResult pass2;
  InvalidHandleResult invalid1;
  InvalidHandleResult invalid2;
};

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
