#pragma once

#include "assertion_helpers.h"
#include "dispatch_cache_contract.h"
#include "probe_reporting.h"

namespace objc3c {
namespace tooling {
namespace runtime_fast_path_contract_probe {

inline int RunRuntimeFastPathContractProbe() {
  const ProbeRun run = CaptureProbeRun();
  WriteProbeReportToStdout(run);
  return ProbeAssertionsPassed(run) ? 0 : 1;
}

} // namespace runtime_fast_path_contract_probe
} // namespace tooling
} // namespace objc3c
