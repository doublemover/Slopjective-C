#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_

#include "import_module_execution_matrix_probe/fixture_module_setup.h"
#include "import_module_execution_matrix_probe/import_execution_cases.h"
#include "import_module_execution_matrix_probe/matrix_assertions.h"
#include "import_module_execution_matrix_probe/probe_report.h"
#include "import_module_execution_matrix_probe/probe_result.h"

namespace objc3c::runtime::probe::import_module_execution_matrix {

inline ProbeResult RunImportModuleExecutionMatrixProbe() {
  ProbeResult result;
  result.startup_fixture = CaptureStartupFixtureModuleState();
  result.startup_execution =
      ExecuteStartupImportExecutionCases(result.startup_fixture);
  result.startup_matrix =
      CaptureStartupMatrixAssertions(result.startup_fixture);
  StabilizeStartupFixtureModuleState(result.startup_fixture);
  StabilizeStartupMatrixAssertions(result.startup_matrix);

  objc3_runtime_reset_for_testing();
  result.post_reset = CapturePostResetRuntimeState();
  result.replay_status = objc3_runtime_replay_registered_images_for_testing();

  result.post_replay_fixture = CapturePostReplayFixtureModuleState();
  result.post_replay_execution =
      ExecutePostReplayImportExecutionCases(result.post_replay_fixture);
  result.post_replay_matrix =
      CapturePostReplayMatrixAssertions(result.post_replay_fixture);
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = RunImportModuleExecutionMatrixProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::import_module_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_
