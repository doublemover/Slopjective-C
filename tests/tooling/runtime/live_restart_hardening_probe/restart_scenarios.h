#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_RESTART_SCENARIOS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_RESTART_SCENARIOS_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime_state_capture.h"

namespace objc3c::runtime::live_restart_hardening_probe {

inline UnsupportedReplayState RunUnsupportedReplayScenario() {
  return CaptureUnsupportedReplayState(
      objc3_runtime_replay_registered_images_for_testing());
}

inline ResetState RunResetScenario() {
  objc3_runtime_reset_for_testing();
  return CaptureResetState();
}

inline RestartState RunRestartScenario() {
  return CaptureRestartState(
      objc3_runtime_replay_registered_images_for_testing());
}

inline RestartCycleState RunRestartCycle() {
  return {
      RunResetScenario(),
      RunRestartScenario(),
  };
}

}  // namespace objc3c::runtime::live_restart_hardening_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_RESTART_SCENARIOS_H_
