#pragma once

#include "fixture_runtime_setup.h"
#include "object_class_assertions.h"
#include "report_helpers.h"
#include "runnable_invocation_assertions.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace runtime_canonical_runnable_object {

inline int RunRuntimeCanonicalRunnableObjectProbe() {
  ProbeRun run;

  CaptureFixtureRuntimeSetup(run.fixture);
  CaptureRunnableInvocationAssertions(run);
  if (!RunnableInvocationAssertionsPassed(run.runnable)) {
    return 1;
  }

  CaptureObjectClassAssertions(run);
  PrintRuntimeCanonicalRunnableObjectReport(run);
  return 0;
}

} // namespace runtime_canonical_runnable_object
} // namespace probe
} // namespace runtime
} // namespace objc3c
