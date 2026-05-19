#pragma once

#include "runtime_snapshot_helpers.h"

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void ResetReferenceCountingRuntimeFixture() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();
}

inline ReferenceCountingFixture AllocateReferenceCountingFixture() {
  ReferenceCountingFixture fixture;
  fixture.parent = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  fixture.child = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  return fixture;
}

inline void RecordReferenceCountingFixtureHandles(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations) {
  operations.parent = fixture.parent;
  operations.child = fixture.child;
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
