#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_RESTART_LIFECYCLE_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_RESTART_LIFECYCLE_ASSERTIONS_H_

#include "bootstrap_fixture_setup.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::bootstrap_failure_restart_probe {

inline ResetState RunResetScenario() {
  objc3_runtime_reset_for_testing();
  return CaptureResetState();
}

inline RestartState RunRestartScenario() {
  return CaptureRestartState(
      objc3_runtime_replay_registered_images_for_testing());
}

inline RestartCycle RunRestartCycle() {
  return {
      RunResetScenario(),
      RunRestartScenario(),
  };
}

}  // namespace objc3c::runtime::bootstrap_failure_restart_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_RESTART_LIFECYCLE_ASSERTIONS_H_
