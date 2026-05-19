#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_ORCHESTRATION_H_

#include "bootstrap_fixture_setup.h"
#include "failure_scenarios.h"
#include "probe_state.h"
#include "restart_lifecycle_assertions.h"

namespace objc3c::runtime::bootstrap_failure_restart_probe {

inline ProbeRun RunBootstrapFailureRestartScenarios() {
  return {
      CaptureBootstrapFixture(),
      RunUnsupportedReplayScenario(),
      RunRestartCycle(),
      RunUnsupportedReplayScenario(),
      RunRestartCycle(),
  };
}

}  // namespace objc3c::runtime::bootstrap_failure_restart_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_ORCHESTRATION_H_
