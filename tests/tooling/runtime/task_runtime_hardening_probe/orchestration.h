#pragma once

#include "hardening_scenarios.h"
#include "probe_state.h"
#include "task_fixture_setup.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline ProbeRun RunTaskRuntimeHardeningScenarios() {
  return ProbeRun{RunPass(kReplayScenario), RunPass(kReplayScenario)};
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
