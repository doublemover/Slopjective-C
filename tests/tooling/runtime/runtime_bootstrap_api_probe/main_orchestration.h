#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_MAIN_ORCHESTRATION_H_

#include "api_call_scenarios.h"
#include "probe_state.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::runtime_bootstrap_api {

inline BootstrapApiProbeResult RunProbeLifecycle() {
  BootstrapApiProbeResult result;
  result.bootstrap = RunBootstrapApiScenario();
  result.reset = RunResetScenario();
  return result;
}

inline int RunProbeMain() {
  const BootstrapApiProbeResult result = RunProbeLifecycle();
  PrintBootstrapApiProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_MAIN_ORCHESTRATION_H_
