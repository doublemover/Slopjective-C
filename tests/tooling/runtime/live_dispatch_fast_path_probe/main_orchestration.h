#pragma once

#include "dispatch_assertions.h"
#include "probe_reporting.h"
#include "runtime_bootstrap.h"

namespace objc3c {
namespace tooling {
namespace live_dispatch_fast_path_probe {

inline int RunLiveDispatchFastPathProbe() {
  const ProbeRun run = CaptureProbeRun();
  WriteProbeReportToStdout(run);
  return ProbeAssertionsPassed(run) ? 0 : 1;
}

} // namespace live_dispatch_fast_path_probe
} // namespace tooling
} // namespace objc3c
