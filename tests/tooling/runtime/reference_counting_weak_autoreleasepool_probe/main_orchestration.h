#pragma once

#include "report_helpers.h"
#include "weak_autoreleasepool_scenarios.h"

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void CaptureReferenceCountingWeakAutoreleasepoolProbe(
    ReferenceCountingWeakAutoreleasepoolRun &run) {
  ResetReferenceCountingRuntimeFixture();
  run.fixture = AllocateReferenceCountingFixture();
  RecordReferenceCountingFixtureHandles(run.fixture, run.operations);

  CaptureStrongReferenceSetup(run.fixture, run.operations,
                              run.snapshots.graph_after_setup);

  CaptureWeakReferenceInsideAutoreleasepool(
      run.fixture, run.operations, run.snapshots.graph_inside_pool,
      run.snapshots.memory_inside_pool);
  CaptureWeakReferenceAfterAutoreleasepool(
      run.fixture, run.operations, run.snapshots.graph_after_pool,
      run.snapshots.memory_after_pool);

  CaptureAfterParentRelease(run.fixture, run.operations,
                            run.snapshots.graph_after_parent_release,
                            run.snapshots.memory_after_parent_release);
  CaptureNestedAutoreleasepoolLifoDrainOrder(
      run.operations, run.snapshots.memory_before_nested_pool,
      run.snapshots.memory_nested_pool, run.snapshots.memory_after_inner_pool,
      run.snapshots.memory_after_outer_pool,
      run.snapshots.memory_after_nested_release_cleanup);
  CaptureWeakValuePropertyEntry(run.snapshots.weak_value_entry);
  CaptureResetCleanupForAllocatedRuntimeInstances(
      run.operations, run.snapshots.memory_before_reset_cleanup,
      run.snapshots.memory_after_reset_cleanup);
}

inline int RunReferenceCountingWeakAutoreleasepoolProbe() {
  ReferenceCountingWeakAutoreleasepoolRun run{};
  CaptureReferenceCountingWeakAutoreleasepoolProbe(run);
  PrintReferenceCountingWeakAutoreleasepoolReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
