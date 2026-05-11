#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_MAIN_ORCHESTRATION_H_

#include "orchestration.h"
#include "report_helpers.h"

namespace objc3c::runtime::bootstrap_failure_restart_probe {

inline int RunProbe() {
  const ProbeRun run = RunBootstrapFailureRestartScenarios();
  PrintProbeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::bootstrap_failure_restart_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_FAILURE_RESTART_PROBE_MAIN_ORCHESTRATION_H_
