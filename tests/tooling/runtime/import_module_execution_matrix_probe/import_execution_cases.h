#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_IMPORT_EXECUTION_CASES_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_IMPORT_EXECUTION_CASES_H_

#include "import_module_execution_matrix_probe/probe_result.h"

namespace objc3c::runtime::probe::import_module_execution_matrix {

inline ImportExecutionCaseValues ExecuteImportExecutionCases(
    int imported_class_receiver, int local_class_receiver) {
  ImportExecutionCaseValues values;
  values.imported_provider_class_value = objc3_runtime_dispatch_i32(
      imported_class_receiver, kProviderClassValueSelector, 0, 0, 0, 0);
  values.imported_provider_protocol_value = objc3_runtime_dispatch_i32(
      imported_class_receiver, kImportedProtocolValueSelector, 0, 0, 0, 0);
  values.local_consumer_class_value = objc3_runtime_dispatch_i32(
      local_class_receiver, kLocalClassValueSelector, 0, 0, 0, 0);
  return values;
}

inline ImportExecutionCaseValues ExecuteStartupImportExecutionCases(
    const StartupFixtureModuleState &state) {
  return ExecuteImportExecutionCases(state.ImportedClassReceiver(),
                                     state.LocalClassReceiver());
}

inline ImportExecutionCaseValues ExecutePostReplayImportExecutionCases(
    const PostReplayFixtureModuleState &state) {
  return ExecuteImportExecutionCases(state.ImportedClassReceiver(),
                                     state.LocalClassReceiver());
}

}  // namespace objc3c::runtime::probe::import_module_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_IMPORT_EXECUTION_CASES_H_
