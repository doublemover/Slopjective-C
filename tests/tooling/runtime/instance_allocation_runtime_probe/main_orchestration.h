#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_

#include "allocation_invariant_assertions.h"
#include "allocation_mutations.h"
#include "allocation_setup.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline ProbeRun CaptureInstanceAllocationRuntimeProbe() {
  ProbeRun run;
  run.registration_state = CaptureRegistrationState();
  run.selector_table_state = CaptureSelectorTableState();
  run.fixture = AllocateWidgetInstances();
  run.mutations = CaptureAllocationMutationResults(run.fixture);
  run.invariants = CaptureAllocationInvariantSnapshots(run.fixture);
  return run;
}

inline int RunInstanceAllocationRuntimeProbe() {
  const ProbeRun run = CaptureInstanceAllocationRuntimeProbe();
  PrintInstanceAllocationRuntimeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
