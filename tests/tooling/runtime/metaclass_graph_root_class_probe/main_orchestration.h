#pragma once

#include "fixture_runtime_setup.h"
#include "metaclass_graph_assertions.h"
#include "report_error_helpers.h"
#include "root_class_invariants.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline int RunMetaclassGraphRootClassProbe() {
  ProbeRun run;
  CaptureRuntimeBootstrapFixture(run.fixture);
  CaptureMetaclassGraphAssertions(run.graph_assertions);
  CaptureRootClassInvariants(run.root_class_invariants);
  PrintMetaclassGraphRootClassProbeReport(run);
  return 0;
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
