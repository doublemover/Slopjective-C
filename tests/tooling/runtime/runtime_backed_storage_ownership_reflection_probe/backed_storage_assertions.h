#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_BACKED_STORAGE_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_BACKED_STORAGE_ASSERTIONS_H_

#include "ownership_reflection_capture.h"
#include "probe_result.h"
#include "storage_fixture_definitions.h"

namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection {

inline void CaptureBackedStorageOwnershipAssertions(
    BackedStorageOwnershipAssertions &assertions) {
  assertions = BackedStorageOwnershipAssertions{};
  CaptureStorageAccessorImplementationSurface(
      assertions.implementation_surface);
  CaptureStorageOwnershipProperty(kCurrentValuePropertyQuery,
                                  assertions.current_value_property);
  CaptureStorageOwnershipProperty(kCopiedValuePropertyQuery,
                                  assertions.copied_value_property);
  CaptureStorageOwnershipProperty(kWeakValuePropertyQuery,
                                  assertions.weak_value_property);
  CaptureStorageOwnershipProperty(kBorrowedValuePropertyQuery,
                                  assertions.borrowed_value_property);
  CaptureStorageOwnershipProperty(kGuardedValuePropertyQuery,
                                  assertions.guarded_value_property);
}

}  // namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_BACKED_STORAGE_ASSERTIONS_H_
