#pragma once

#include "probe_state.h"
#include "support/typed_dispatch_helpers.h"

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
  operations.strong_set_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
      fixture.parent, "setCurrentValue:", fixture.child, 0, 0, 0);
  operations.weak_set_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
      fixture.parent, "setWeakValue:", fixture.child, 0, 0, 0);
  operations.weak_before_clear =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "weakValue");
  operations.retain_result = objc3_runtime_retain_i32(fixture.child);
  operations.release_after_retain_result =
      objc3_runtime_release_i32(fixture.child);
  operations.release_local_result = objc3_runtime_release_i32(fixture.child);
}

inline void CaptureStrongClearOwnershipResults(
    const OwnershipFixture &fixture,
    OwnershipOperationResults &operations) {
  operations.strong_before_clear =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "currentValue");
  operations.clear_strong_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
          fixture.parent, "setCurrentValue:", 0);
  operations.strong_after_clear =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "currentValue");
  operations.weak_after_clear =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "weakValue");
}

inline void CaptureParentReleaseOwnershipResult(
    const OwnershipFixture &fixture,
    OwnershipOperationResults &operations) {
  operations.parent_release_result = objc3_runtime_release_i32(fixture.parent);
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
