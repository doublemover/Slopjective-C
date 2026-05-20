#pragma once

#include "runtime_snapshot_helpers.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void ResetReferenceCountingRuntimeFixture() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();
}

inline ReferenceCountingFixture AllocateReferenceCountingFixture() {
  ReferenceCountingFixture fixture;
  fixture.parent =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  fixture.child =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(1024, "alloc");
  return fixture;
}

inline void RecordReferenceCountingFixtureHandles(
    const ReferenceCountingFixture &fixture,
    ReferenceCountingOperationResults &operations) {
  operations.parent = fixture.parent;
  operations.child = fixture.child;
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
