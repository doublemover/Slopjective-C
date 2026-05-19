#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_

#include "lookup_reflection_capture.h"
#include "object_model_fixture_setup.h"
#include "report_helpers.h"
#include "runtime_assertion_helpers.h"

namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime {

inline void CaptureObjectModelLookupReflectionRuntimeProbe(
    ProbeResult &result) {
  result = ProbeResult{};
  CaptureObjectModelFixture(result.fixture);
  CaptureWidgetRuntimeDispatchAssertions(result.fixture, result.dispatch);
  CaptureLookupReflectionState(result.reflection);
}

inline int RunProbeMain() {
  ProbeResult result{};
  CaptureObjectModelLookupReflectionRuntimeProbe(result);
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
