#pragma once

#include "realization_actions.h"
#include "report_helpers.h"
#include "runtime_fixture_setup.h"

namespace objc3c::runtime::probe::class_realization_runtime {

inline int RunClassRealizationRuntimeProbe() {
  ClassRealizationProbeRun run;
  CaptureClassRealizationActions(run);
  CaptureRuntimeRegistryFixture(run.registry);
  PrintClassRealizationRuntimeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::class_realization_runtime
