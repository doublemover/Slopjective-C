#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_MAIN_ORCHESTRATION_H_

#include "report_helpers.h"
#include "runnable_sample_setup.h"
#include "runtime_metadata_assertions.h"
#include "sample_execution_assertions.h"

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline void StabilizeCanonicalRunnableSampleSetProbe(ProbeResult &result) {
  StabilizeRunnableSampleFixture(result.fixture);
  StabilizeRunnableSampleAssertions(result.assertions);
}

inline void CaptureCanonicalRunnableSampleSetProbe(ProbeResult &result) {
  result = ProbeResult{};
  CaptureRunnableSampleFixture(result.fixture);
  result.execution = ExecuteRunnableSampleSet(result.fixture);
  CaptureRunnableSampleAssertions(result.assertions);
}

inline int RunProbeMain() {
  ProbeResult result;
  CaptureCanonicalRunnableSampleSetProbe(result);
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_MAIN_ORCHESTRATION_H_
