#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_MAIN_ORCHESTRATION_H_

#include "cross_module_runtime_packaging_probe/cross_module_runtime_assertions.h"
#include "cross_module_runtime_packaging_probe/package_module_fixture.h"
#include "cross_module_runtime_packaging_probe/report_error_helpers.h"

namespace objc3c::runtime::probe::cross_module_runtime_packaging {

inline ProbeResult RunCrossModuleRuntimePackagingProbe() {
  ProbeResult result;
  result.startup = CaptureStartupPackageModuleState();
  result.startup_assertions = CaptureStartupRuntimeAssertions(result.startup);

  objc3_runtime_reset_for_testing();
  result.post_reset = CapturePostResetRuntimeState();
  result.replay_status = objc3_runtime_replay_registered_images_for_testing();

  result.post_replay = CapturePostReplayPackageModuleState();
  result.post_replay_dispatch_values =
      ExecutePostReplayRuntimeDispatches(result.post_replay);
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = RunCrossModuleRuntimePackagingProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::cross_module_runtime_packaging

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_MAIN_ORCHESTRATION_H_
