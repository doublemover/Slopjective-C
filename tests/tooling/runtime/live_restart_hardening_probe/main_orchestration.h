#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_MAIN_ORCHESTRATION_H_

#include "lifecycle_assertions.h"
#include "report_helpers.h"

namespace objc3c::runtime::live_restart_hardening_probe {

inline int RunProbe() {
  const ProbeRun run = CaptureLiveRestartHardeningRun();
  PrintProbeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::live_restart_hardening_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_MAIN_ORCHESTRATION_H_
