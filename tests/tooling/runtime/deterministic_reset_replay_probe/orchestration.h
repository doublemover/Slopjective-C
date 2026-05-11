#pragma once

#include "probe_result.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::deterministic_reset_replay {

inline ProbeResult RunProbeLifecycle() {
  ProbeResult result;
  result.startup = CaptureStartupState();
  result.reset_replay = RunResetReplayCycle();
  return result;
}

inline int RunProbe() {
  const ProbeResult result = RunProbeLifecycle();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::deterministic_reset_replay
