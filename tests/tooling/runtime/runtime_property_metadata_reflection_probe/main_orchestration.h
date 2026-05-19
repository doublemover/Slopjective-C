#pragma once

#include "fixture_runtime_setup.h"
#include "reflection_assertions.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

inline void CaptureRuntimePropertyMetadataReflectionProbe(ProbeResult &result) {
  CaptureRegistryStateBeforeReflection(result.assertions);
  CaptureRuntimeReflectionFixture(result.fixture);
  CaptureDeclaredPropertyReflections(result.assertions);
  CaptureRegistryStateAfterCountReflection(result.assertions);
  CaptureMissingPropertyReflections(result.assertions);
  CaptureRegistryStateAfterMissingReflection(result.assertions);
}

inline int RunProbeMain() {
  ProbeResult result;
  CaptureRuntimePropertyMetadataReflectionProbe(result);
  PrintRuntimePropertyMetadataReflectionReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
