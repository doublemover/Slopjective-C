#pragma once

#include "probe_state.h"

#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void PrintReferenceCountingWeakAutoreleasepoolReport(
    const ReferenceCountingWeakAutoreleasepoolRun &run) {
  const ReferenceCountingOperationResults &operations = run.operations;
  const ReferenceCountingSnapshotCaptures &snapshots = run.snapshots;

  std::printf("{");
  std::printf("\"parent\":%d,", operations.parent);
  std::printf("\"child\":%d,", operations.child);
  std::printf("\"strong_set_result\":%d,", operations.strong_set_result);
  std::printf("\"release_local_result\":%d,",
              operations.release_local_result);
  std::printf("\"getter_value\":%d,", operations.getter_value);
  std::printf("\"weak_set_result\":%d,", operations.weak_set_result);
  std::printf("\"clear_strong_result\":%d,",
              operations.clear_strong_result);
  std::printf("\"weak_inside_pool\":%d,", operations.weak_inside_pool);
  std::printf("\"weak_after_pool\":%d,", operations.weak_after_pool);
  std::printf("\"parent_release_result\":%d,",
              operations.parent_release_result);
  std::printf("\"graph_after_setup\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      snapshots.graph_after_setup.snapshot);
  std::printf(",\"graph_inside_pool\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      snapshots.graph_inside_pool.snapshot);
  std::printf(",\"graph_after_pool\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      snapshots.graph_after_pool.snapshot);
  std::printf(",\"graph_after_parent_release\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      snapshots.graph_after_parent_release.snapshot);
  std::printf(",\"memory_inside_pool\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_inside_pool.snapshot);
  std::printf(",\"memory_after_pool\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_after_pool.snapshot);
  std::printf(",\"memory_after_parent_release\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_after_parent_release.snapshot);
  std::printf(",\"weak_value_entry\":");
  ::objc3c::runtime::probe::PrintPropertyEntryWeakAutoreleasepool(
      snapshots.weak_value_entry.snapshot);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
