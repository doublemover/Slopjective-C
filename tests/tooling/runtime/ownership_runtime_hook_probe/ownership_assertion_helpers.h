#pragma once

#include "probe_state.h"

namespace objc3c::runtime::probe::ownership_runtime_hook {

inline void RecordOwnershipFixtureHandles(
    const OwnershipFixture &fixture,
    OwnershipOperationResults &operations) {
  operations.parent = fixture.parent;
  operations.child = fixture.child;
}

inline void CaptureInitialOwnershipHookResults(
    const OwnershipFixture &fixture,
    OwnershipOperationResults &operations) {
  operations.strong_set_result = objc3_runtime_dispatch_i32(
      fixture.parent, "setCurrentValue:", fixture.child, 0, 0, 0);
  operations.weak_set_result = objc3_runtime_dispatch_i32(
      fixture.parent, "setWeakValue:", fixture.child, 0, 0, 0);
  operations.weak_before_clear =
      objc3_runtime_dispatch_i32(fixture.parent, "weakValue", 0, 0, 0, 0);
  operations.retain_result = objc3_runtime_retain_i32(fixture.child);
  operations.release_after_retain_result =
      objc3_runtime_release_i32(fixture.child);
  operations.release_local_result = objc3_runtime_release_i32(fixture.child);
}

inline void CaptureStrongClearOwnershipResults(
    const OwnershipFixture &fixture,
    OwnershipOperationResults &operations) {
  operations.strong_before_clear =
      objc3_runtime_dispatch_i32(fixture.parent, "currentValue", 0, 0, 0, 0);
  operations.clear_strong_result =
      objc3_runtime_dispatch_i32(fixture.parent, "setCurrentValue:", 0, 0, 0, 0);
  operations.strong_after_clear =
      objc3_runtime_dispatch_i32(fixture.parent, "currentValue", 0, 0, 0, 0);
  operations.weak_after_clear =
      objc3_runtime_dispatch_i32(fixture.parent, "weakValue", 0, 0, 0, 0);
}

inline void CaptureParentReleaseOwnershipResult(
    const OwnershipFixture &fixture,
    OwnershipOperationResults &operations) {
  operations.parent_release_result = objc3_runtime_release_i32(fixture.parent);
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
