#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_FAILURE_SCENARIOS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_FAILURE_SCENARIOS_H_

#include "bootstrap_fixture_setup.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::bootstrap_failure_restart_probe {

inline UnsupportedReplayScenario RunUnsupportedReplayScenario() {
  return CaptureUnsupportedReplayScenario(
      objc3_runtime_replay_registered_images_for_testing());
}

}  // namespace objc3c::runtime::bootstrap_failure_restart_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_FAILURE_SCENARIOS_H_
