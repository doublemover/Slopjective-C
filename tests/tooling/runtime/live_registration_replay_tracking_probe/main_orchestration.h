#pragma once

#include "report_error_helpers.h"

namespace objc3c::runtime::probe::live_registration_replay_tracking {

inline ProbeResult RunLiveRegistrationReplayTrackingProbe() {
  ProbeResult result;
  result.startup = CaptureStartupReplayTrackingSnapshot();
  result.replay = RunResetReplayTrackingCycle();
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = RunLiveRegistrationReplayTrackingProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::live_registration_replay_tracking
