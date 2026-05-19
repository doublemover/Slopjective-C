#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_RUNTIME_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_RUNTIME_ASSERTION_HELPERS_H_

#include "probe_state.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::runtime_memory_management_api {

inline void RecordMemoryManagementFixtureHandles(
    const MemoryManagementFixture &fixture,
    MemoryManagementOperationResults &operations) {
  operations.parent = fixture.parent;
  operations.child = fixture.child;
}

inline void CaptureRelationshipAssignmentResults(
    const MemoryManagementFixture &fixture,
    MemoryManagementOperationResults &operations) {
  operations.strong_set_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
      fixture.parent, "setCurrentValue:", fixture.child, 0, 0, 0);
  operations.weak_set_result =
      ::objc3c::runtime::probe::DispatchTypedStatus(
      fixture.parent, "setWeakValue:", fixture.child, 0, 0, 0);
}

inline void CaptureHelperOwnershipResults(
    const MemoryManagementFixture &fixture,
    MemoryManagementOperationResults &operations) {
  operations.retain_result = objc3_runtime_retain_i32(fixture.child);
  operations.autorelease_result = objc3_runtime_autorelease_i32(fixture.child);
  operations.release_after_helper_result =
      objc3_runtime_release_i32(fixture.child);
  operations.release_local_result = objc3_runtime_release_i32(fixture.child);
}

inline void CapturePreClearReadResults(
    const MemoryManagementFixture &fixture,
    MemoryManagementOperationResults &operations) {
  operations.strong_before_clear =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "currentValue");
  operations.weak_before_clear =
      ::objc3c::runtime::probe::DispatchTypedObjectReference(
          fixture.parent, "weakValue");
}

inline void CaptureStrongClearResults(
    const MemoryManagementFixture &fixture,
    MemoryManagementOperationResults &operations) {
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

inline void CaptureParentReleaseResult(
    const MemoryManagementFixture &fixture,
    MemoryManagementOperationResults &operations) {
  operations.parent_release_result = objc3_runtime_release_i32(fixture.parent);
}

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_RUNTIME_ASSERTION_HELPERS_H_
