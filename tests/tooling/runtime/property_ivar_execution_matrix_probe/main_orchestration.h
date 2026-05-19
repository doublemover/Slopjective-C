#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_

#include "execution_assertions.h"
#include "fixture_setup.h"
#include "probe_report.h"
#include "property_ivar_matrix_cases.h"

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline void CapturePropertyIvarExecutionMatrixProbe(ProbeResult &result) {
  result = ProbeResult{};
  result.fixture = SetUpWidgetFixture();
  ExecutePropertyIvarMatrixCases(result.fixture.widget_instance,
                                 result.execution);
  CaptureWidgetFixtureEntry(result.fixture);
  CapturePropertyIvarExecutionAssertions(result.fixture.widget_instance,
                                         result.assertions);
}

inline int RunProbeMain() {
  ProbeResult result{};
  CapturePropertyIvarExecutionMatrixProbe(result);
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_
