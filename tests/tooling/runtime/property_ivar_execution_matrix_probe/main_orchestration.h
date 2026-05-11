#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_

#include "execution_assertions.h"
#include "fixture_setup.h"
#include "probe_report.h"
#include "property_ivar_matrix_cases.h"

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline ProbeResult CapturePropertyIvarExecutionMatrixProbe() {
  ProbeResult result;
  result.fixture = SetUpWidgetFixture();
  result.execution =
      ExecutePropertyIvarMatrixCases(result.fixture.widget_instance);
  CaptureWidgetFixtureEntry(result.fixture);
  result.assertions =
      CapturePropertyIvarExecutionAssertions(result.fixture.widget_instance);
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = CapturePropertyIvarExecutionMatrixProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_MAIN_ORCHESTRATION_H_
