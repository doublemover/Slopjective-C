#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_MEMORY_MANAGEMENT_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_MEMORY_MANAGEMENT_FIXTURE_SETUP_H_

#include "runtime_snapshot_helpers.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::runtime_memory_management_api {

inline constexpr int kMemoryManagementAllocatorReceiver = 1024;

inline void ResetMemoryManagementRuntimeFixture() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();
}

inline MemoryManagementFixture AllocateMemoryManagementFixture(
    RealizedClassGraphCapture &graph_after_alloc) {
  MemoryManagementFixture fixture;
  fixture.parent = ::objc3c::runtime::probe::DispatchTypedObjectReference(
      kMemoryManagementAllocatorReceiver, "alloc");
  fixture.child = ::objc3c::runtime::probe::DispatchTypedObjectReference(
      kMemoryManagementAllocatorReceiver, "alloc");
  CaptureRealizedClassGraph(graph_after_alloc);
  return fixture;
}

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_MEMORY_MANAGEMENT_FIXTURE_SETUP_H_
