#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_ORCHESTRATION_H_

#include "probe_report.h"
#include "probe_result.h"

namespace objc3c::runtime::probe::multi_image_registration_reset_replay {

inline ProbeResult RunProbeLifecycle() {
  ProbeResult result;
  result.startup = CaptureRuntimeImageLifecycleState();
  result.first_cycle = RunResetReplayCycle();
  result.blocked_replay = RunBlockedReplayAssertion();
  result.second_cycle = RunResetReplayCycle();
  return result;
}

inline int RunProbe() {
  const ProbeResult result = RunProbeLifecycle();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::multi_image_registration_reset_replay

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_ORCHESTRATION_H_
