#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::ownership_runtime_hook {

inline void PrintOwnershipRuntimeHookReport(const OwnershipProbeRun &run) {
  const OwnershipOperationResults &operations = run.operations;

  std::printf("{");
  std::printf("\"parent\":%d,", operations.parent);
  std::printf("\"child\":%d,", operations.child);
  std::printf("\"strong_set_result\":%d,", operations.strong_set_result);
  std::printf("\"weak_set_result\":%d,", operations.weak_set_result);
  std::printf("\"weak_before_clear\":%d,", operations.weak_before_clear);
  std::printf("\"retain_result\":%d,", operations.retain_result);
  std::printf("\"release_after_retain_result\":%d,",
              operations.release_after_retain_result);
  std::printf("\"release_local_result\":%d,", operations.release_local_result);
  std::printf("\"strong_before_clear\":%d,", operations.strong_before_clear);
  std::printf("\"clear_strong_result\":%d,", operations.clear_strong_result);
  std::printf("\"strong_after_clear\":%d,", operations.strong_after_clear);
  std::printf("\"weak_after_clear\":%d,", operations.weak_after_clear);
  std::printf("\"parent_release_result\":%d,",
              operations.parent_release_result);
  std::printf("\"graph_after_alloc\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_alloc.snapshot);
  std::printf(",\"graph_after_drop_local\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_drop_local.snapshot);
  std::printf(",\"graph_after_clear\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_clear.snapshot);
  std::printf(",\"graph_after_parent_release\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_parent_release.snapshot);
  std::printf(",\"current_value_entry\":");
  ::objc3c::runtime::probe::PrintPropertyEntryOwnershipHook(
      run.current_value_entry.entry);
  std::printf(",\"weak_value_entry\":");
  ::objc3c::runtime::probe::PrintPropertyEntryOwnershipHook(
      run.weak_value_entry.entry);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
