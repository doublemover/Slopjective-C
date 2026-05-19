#pragma once

#include "reference_counting_fixture_setup.h"

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void CaptureStrongReferenceSetup(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations,
    RealizedClassGraphCapture &graph_after_setup) {
  operations.strong_set_result = objc3_runtime_dispatch_i32(
      fixture.parent, "setCurrentValue:", fixture.child, 0, 0, 0);
  operations.release_local_result = objc3_runtime_release_i32(fixture.child);
  CaptureRealizedClassGraph(graph_after_setup);
}

inline void CaptureWeakReferenceInsideAutoreleasepool(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations,
    RealizedClassGraphCapture &graph_inside_pool,
    MemoryManagementCapture &memory_inside_pool) {
  objc3_runtime_push_autoreleasepool_scope();
  operations.getter_value =
      objc3_runtime_dispatch_i32(fixture.parent, "currentValue", 0, 0, 0, 0);
  operations.weak_set_result = objc3_runtime_dispatch_i32(
      fixture.parent, "setWeakValue:", operations.getter_value, 0, 0, 0);
  operations.clear_strong_result = objc3_runtime_dispatch_i32(
      fixture.parent, "setCurrentValue:", 0, 0, 0, 0);
  operations.weak_inside_pool =
      objc3_runtime_dispatch_i32(fixture.parent, "weakValue", 0, 0, 0, 0);
  CaptureRealizedClassGraph(graph_inside_pool);
  CaptureMemoryManagementState(memory_inside_pool);
}

inline void CaptureWeakReferenceAfterAutoreleasepool(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations,
    RealizedClassGraphCapture &graph_after_pool,
    MemoryManagementCapture &memory_after_pool) {
  objc3_runtime_pop_autoreleasepool_scope();
  operations.weak_after_pool =
      objc3_runtime_dispatch_i32(fixture.parent, "weakValue", 0, 0, 0, 0);
  CaptureRealizedClassGraph(graph_after_pool);
  CaptureMemoryManagementState(memory_after_pool);
}

inline void CaptureAfterParentRelease(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations,
    RealizedClassGraphCapture &graph_after_parent_release,
    MemoryManagementCapture &memory_after_parent_release) {
  operations.parent_release_result = objc3_runtime_release_i32(fixture.parent);
  CaptureRealizedClassGraph(graph_after_parent_release);
  CaptureMemoryManagementState(memory_after_parent_release);
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
