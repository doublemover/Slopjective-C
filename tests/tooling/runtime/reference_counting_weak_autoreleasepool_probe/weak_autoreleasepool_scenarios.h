#pragma once

#include "reference_counting_fixture_setup.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void CaptureStrongReferenceSetup(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations,
    RealizedClassGraphCapture &graph_after_setup) {
  operations.strong_set_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
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
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "currentValue");
  operations.weak_set_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
      fixture.parent, "setWeakValue:", operations.getter_value, 0, 0, 0);
  operations.clear_strong_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
          fixture.parent, "setCurrentValue:", 0);
  operations.weak_inside_pool =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "weakValue");
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
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "weakValue");
  CaptureRealizedClassGraph(graph_after_pool);
  CaptureMemoryManagementState(memory_after_pool);
  operations.weak_stale_zeroed =
      operations.getter_value != 0 &&
      operations.weak_inside_pool == operations.getter_value &&
      operations.weak_after_pool == 0;
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

inline void CaptureNestedAutoreleasepoolLifoDrainOrder(
    ReferenceCountingOperationResults &operations,
    MemoryManagementCapture &memory_before_nested_pool,
    MemoryManagementCapture &memory_nested_pool,
    MemoryManagementCapture &memory_after_inner_pool,
    MemoryManagementCapture &memory_after_outer_pool,
    MemoryManagementCapture &memory_after_nested_release_cleanup) {
  CaptureMemoryManagementState(memory_before_nested_pool);

  operations.nested_outer_retained =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  operations.nested_inner_retained =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");

  objc3_runtime_push_autoreleasepool_scope();
  operations.nested_outer_autoreleased =
      objc3_runtime_autorelease_i32(operations.nested_outer_retained);
  objc3_runtime_push_autoreleasepool_scope();
  operations.nested_inner_autoreleased =
      objc3_runtime_autorelease_i32(operations.nested_inner_retained);
  CaptureMemoryManagementState(memory_nested_pool);

  objc3_runtime_pop_autoreleasepool_scope();
  CaptureMemoryManagementState(memory_after_inner_pool);

  objc3_runtime_pop_autoreleasepool_scope();
  CaptureMemoryManagementState(memory_after_outer_pool);

  CaptureMemoryManagementState(memory_after_nested_release_cleanup);

  const auto drained_before =
      memory_before_nested_pool.snapshot.drained_autorelease_value_count;
  const auto drained_after_inner =
      memory_after_inner_pool.snapshot.drained_autorelease_value_count;
  const auto drained_after_outer =
      memory_after_outer_pool.snapshot.drained_autorelease_value_count;

  operations.nested_lifo_drain_order_observed =
      memory_nested_pool.snapshot.autoreleasepool_depth ==
          memory_before_nested_pool.snapshot.autoreleasepool_depth + 2 &&
      memory_after_inner_pool.snapshot.autoreleasepool_depth ==
          memory_before_nested_pool.snapshot.autoreleasepool_depth + 1 &&
      memory_after_outer_pool.snapshot.autoreleasepool_depth ==
          memory_before_nested_pool.snapshot.autoreleasepool_depth &&
      drained_after_inner == drained_before + 1 &&
      drained_after_outer == drained_before + 2 &&
      memory_after_inner_pool.snapshot.last_drained_autorelease_value ==
          operations.nested_inner_autoreleased &&
      memory_after_outer_pool.snapshot.last_drained_autorelease_value ==
          operations.nested_outer_autoreleased &&
      memory_after_inner_pool.snapshot.live_runtime_instance_count + 1 ==
          memory_nested_pool.snapshot.live_runtime_instance_count &&
      memory_after_outer_pool.snapshot.live_runtime_instance_count + 2 ==
          memory_nested_pool.snapshot.live_runtime_instance_count &&
      memory_after_nested_release_cleanup.snapshot.live_runtime_instance_count ==
          memory_after_outer_pool.snapshot.live_runtime_instance_count;
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
