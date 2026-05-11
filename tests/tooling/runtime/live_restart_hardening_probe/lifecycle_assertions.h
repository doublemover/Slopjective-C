#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_LIFECYCLE_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_LIFECYCLE_ASSERTIONS_H_

#include "probe_state.h"
#include "restart_scenarios.h"
#include "runtime_state_capture.h"

namespace objc3c::runtime::live_restart_hardening_probe {

inline ProbeRun CaptureLiveRestartHardeningRun() {
  return {
      CaptureBootstrapState(),
      RunUnsupportedReplayScenario(),
      RunRestartCycle(),
      RunUnsupportedReplayScenario(),
      RunRestartCycle(),
  };
}

}  // namespace objc3c::runtime::live_restart_hardening_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_LIFECYCLE_ASSERTIONS_H_
