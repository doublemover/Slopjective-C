#pragma once

#include <iostream>
#include <ostream>

#include "probe_state.h"
#include "runtime_assertion_helpers.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline void WriteProbeReport(const ProbeRun &run, std::ostream &out) {
  const PassResult &pass1 = run.pass1;

  out << "pass1_copy_task_status=" << pass1.copy_task_status << "\n";
  out << "pass1_copy_memory_status=" << pass1.copy_memory_status << "\n";
  out << "pass1_copy_arc_status=" << pass1.copy_arc_status << "\n";
  out << "pass1_spawn_group=" << pass1.spawn_group << "\n";
  out << "pass1_wait_next=" << pass1.wait_next << "\n";
  out << "pass1_cancel_all=" << pass1.cancel_all << "\n";
  out << "pass1_spawn_call_count=" << pass1.task.spawn_call_count << "\n";
  out << "pass1_cancel_all_call_count="
      << pass1.task.cancel_all_call_count << "\n";
  out << "pass1_executor_hop_call_count="
      << pass1.task.executor_hop_call_count << "\n";
  out << "pass1_last_executor_hop_value="
      << pass1.task.last_executor_hop_value << "\n";
  out << "pass1_autoreleasepool_depth="
      << pass1.memory.autoreleasepool_depth << "\n";
  out << "pass1_autoreleasepool_max_depth="
      << pass1.memory.autoreleasepool_max_depth << "\n";
  out << "pass1_autoreleasepool_push_count="
      << pass1.arc.autoreleasepool_push_count << "\n";
  out << "pass1_autoreleasepool_pop_count="
      << pass1.arc.autoreleasepool_pop_count << "\n";
  out << "replay_equal=" << (Equivalent(run.pass1, run.pass2) ? 1 : 0)
      << "\n";
}

inline void WriteProbeReportToStdout(const ProbeRun &run) {
  WriteProbeReport(run, std::cout);
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
