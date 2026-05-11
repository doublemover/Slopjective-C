#pragma once

#include "hook_state_capture.h"

namespace objc3c::runtime::probe::ownership_runtime_hook {

inline void ResetOwnershipRuntimeFixture() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();
}

inline OwnershipFixture AllocateOwnershipFixture(
    RealizedClassGraphCapture &graph_after_alloc) {
  OwnershipFixture fixture;
  fixture.parent = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  fixture.child = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  CaptureRealizedClassGraph(graph_after_alloc);
  return fixture;
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
