#pragma once

#include "hook_state_capture.h"
#include "ownership_assertion_helpers.h"
#include "ownership_fixture_setup.h"
#include "report_error_helpers.h"

namespace objc3c::runtime::probe::ownership_runtime_hook {

inline OwnershipProbeRun CaptureOwnershipRuntimeHookProbe() {
  OwnershipProbeRun run;

  ResetOwnershipRuntimeFixture();
  const OwnershipFixture fixture =
      AllocateOwnershipFixture(run.graphs.after_alloc);
  RecordOwnershipFixtureHandles(fixture, run.operations);

  CaptureInitialOwnershipHookResults(fixture, run.operations);
  CaptureRealizedClassGraph(run.graphs.after_drop_local);

  CaptureStrongClearOwnershipResults(fixture, run.operations);
  CaptureRealizedClassGraph(run.graphs.after_clear);

  CaptureParentReleaseOwnershipResult(fixture, run.operations);
  CaptureRealizedClassGraph(run.graphs.after_parent_release);

  CapturePropertyEntryOwnershipHook("Box", "currentValue",
                                    run.current_value_entry);
  CapturePropertyEntryOwnershipHook("Box", "weakValue", run.weak_value_entry);
  return run;
}

inline int RunOwnershipRuntimeHookProbe() {
  const OwnershipProbeRun run = CaptureOwnershipRuntimeHookProbe();
  PrintOwnershipRuntimeHookReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
