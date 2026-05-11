#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_MAIN_ORCHESTRATION_H_

#include "backed_storage_assertions.h"
#include "ownership_reflection_capture.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection {

inline ProbeResult CaptureRuntimeBackedStorageOwnershipReflectionProbe() {
  ProbeResult result;
  CaptureRealizedBoxEntry(result.fixture);
  CaptureBackedStorageOwnershipAssertions(result.assertions);
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result =
      CaptureRuntimeBackedStorageOwnershipReflectionProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_MAIN_ORCHESTRATION_H_
