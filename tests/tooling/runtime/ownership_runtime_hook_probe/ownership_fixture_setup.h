#pragma once

#include "hook_state_capture.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::ownership_runtime_hook {

inline void ResetOwnershipRuntimeFixture() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();
}

inline OwnershipFixture AllocateOwnershipFixture(
    RealizedClassGraphCapture &graph_after_alloc) {
  OwnershipFixture fixture;
  fixture.parent =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  fixture.child =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  CaptureRealizedClassGraph(graph_after_alloc);
  return fixture;
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
