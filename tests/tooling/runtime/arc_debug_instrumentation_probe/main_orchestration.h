#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_MAIN_ORCHESTRATION_H_

#include "arc_fixture_setup.h"
#include "instrumentation_orchestration.h"
#include "report_error_helpers.h"

namespace objc3c::runtime::probe::arc_debug_instrumentation {

inline void CaptureArcDebugInstrumentationProbe(ProbeRun *run) {
  ResetAndReplayRuntimeForArcDebugProbe();

  run->fixture = SetUpArcDebugFixture();
  CaptureArcDebugInstrumentation(run);
}

inline int RunArcDebugInstrumentationProbe() {
  ProbeRun run;
  CaptureArcDebugInstrumentationProbe(&run);
  PrintArcDebugInstrumentationReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_MAIN_ORCHESTRATION_H_
