#pragma once

#include "image_walk_capture.h"
#include "invariant_assertion_helpers.h"
#include "report_error_helpers.h"

namespace objc3c::runtime::probe::runtime_registrar_image_walk {

inline ProbeResult CaptureRuntimeRegistrarImageWalkProbe() {
  return {
      CaptureRegistrationObservation(),
      CaptureImageWalkObservation(),
      CaptureSelectorInvariantObservation(),
  };
}

inline int RunProbeMain() {
  const ProbeResult result = CaptureRuntimeRegistrarImageWalkProbe();
  PrintRuntimeRegistrarImageWalkReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_registrar_image_walk
