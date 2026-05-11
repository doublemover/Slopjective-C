#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_MAIN_ORCHESTRATION_H_

#include "accessor_assertion_helpers.h"
#include "report_helpers.h"
#include "synthesized_accessor_actions.h"
#include "synthesized_accessor_setup.h"

namespace objc3c::runtime::probe::synthesized_accessor {

inline ProbeResult CaptureSynthesizedAccessorProbe() {
  ProbeResult result;
  CaptureSynthesizedAccessorSetup(result.setup);
  result.actions = ExecuteSynthesizedAccessorActions();
  result.assertions =
      CaptureAccessorAssertions(result.actions.widget_instance);
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = CaptureSynthesizedAccessorProbe();
  PrintSynthesizedAccessorReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_MAIN_ORCHESTRATION_H_
