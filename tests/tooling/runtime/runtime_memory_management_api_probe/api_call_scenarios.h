#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_API_CALL_SCENARIOS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_API_CALL_SCENARIOS_H_

#include "memory_management_fixture_setup.h"
#include "runtime_assertion_helpers.h"

namespace objc3c::runtime::probe::runtime_memory_management_api {

inline void CaptureMemoryManagementApiProbe(MemoryManagementProbeRun &run) {
  ResetMemoryManagementRuntimeFixture();
  CaptureRegistrationState(run.registration_state);

  const MemoryManagementFixture fixture =
      AllocateMemoryManagementFixture(run.graphs.after_alloc);
  RecordMemoryManagementFixtureHandles(fixture, run.operations);

  CaptureRelationshipAssignmentResults(fixture, run.operations);
  CaptureHelperOwnershipResults(fixture, run.operations);
  CaptureRealizedClassGraph(run.graphs.after_helper_release);

  CapturePreClearReadResults(fixture, run.operations);
  CaptureStrongClearResults(fixture, run.operations);
  CaptureRealizedClassGraph(run.graphs.after_clear);

  CaptureParentReleaseResult(fixture, run.operations);
  CaptureRealizedClassGraph(run.graphs.after_parent_release);
}

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_API_CALL_SCENARIOS_H_
