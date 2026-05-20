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
  std::printf("\"weak_stale_zeroed\":%d,", operations.weak_stale_zeroed);
  std::printf("\"parent_release_result\":%d,",
              operations.parent_release_result);
  std::printf("\"nested_outer_retained\":%d,",
              operations.nested_outer_retained);
  std::printf("\"nested_inner_retained\":%d,",
              operations.nested_inner_retained);
  std::printf("\"nested_outer_autoreleased\":%d,",
              operations.nested_outer_autoreleased);
  std::printf("\"nested_inner_autoreleased\":%d,",
              operations.nested_inner_autoreleased);
  std::printf("\"nested_lifo_drain_order_observed\":%d,",
              operations.nested_lifo_drain_order_observed);
  std::printf("\"reset_parent\":%d,", operations.reset_parent);
  std::printf("\"reset_child\":%d,", operations.reset_child);
  std::printf("\"reset_strong_set_result\":%d,",
              operations.reset_strong_set_result);
  std::printf("\"reset_weak_set_result\":%d,",
              operations.reset_weak_set_result);
  std::printf("\"reset_release_local_result\":%d,",
              operations.reset_release_local_result);
  std::printf("\"reset_cleanup_observed\":%d,",
              operations.reset_cleanup_observed);
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
  std::printf(",\"memory_before_nested_pool\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_before_nested_pool.snapshot);
  std::printf(",\"memory_nested_pool\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_nested_pool.snapshot);
  std::printf(",\"memory_after_inner_pool\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_after_inner_pool.snapshot);
  std::printf(",\"memory_after_outer_pool\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_after_outer_pool.snapshot);
  std::printf(",\"memory_after_nested_release_cleanup\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_after_nested_release_cleanup.snapshot);
  std::printf(",\"weak_value_entry\":");
  ::objc3c::runtime::probe::PrintPropertyEntryWeakAutoreleasepool(
      snapshots.weak_value_entry.snapshot);
  std::printf(",\"memory_before_reset_cleanup\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_before_reset_cleanup.snapshot);
  std::printf(",\"memory_after_reset_cleanup\":");
  ::objc3c::runtime::probe::PrintMemoryManagementState(
      snapshots.memory_after_reset_cleanup.snapshot);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
