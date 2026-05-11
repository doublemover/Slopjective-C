#pragma once

#include "orchestration.h"
#include "report_helpers.h"
#include "runtime_assertion_helpers.h"

namespace objc3c {
namespace tooling {
namespace task_runtime_hardening_probe {

inline int RunProbe() {
  const ProbeRun run = RunTaskRuntimeHardeningScenarios();
  WriteProbeReportToStdout(run);
  return ProbeAssertionsPassed(run) ? 0 : 1;
}

} // namespace task_runtime_hardening_probe
} // namespace tooling
} // namespace objc3c
