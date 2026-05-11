#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_MAIN_ORCHESTRATION_H_

#include "report_helpers.h"
#include "runnable_sample_setup.h"
#include "runtime_metadata_assertions.h"
#include "sample_execution_assertions.h"

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline ProbeResult CaptureCanonicalRunnableSampleSetProbe() {
  ProbeResult result;
  result.fixture = CaptureRunnableSampleFixture();
  result.execution = ExecuteRunnableSampleSet(result.fixture);
  result.assertions = CaptureRunnableSampleAssertions();
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = CaptureCanonicalRunnableSampleSetProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_MAIN_ORCHESTRATION_H_
