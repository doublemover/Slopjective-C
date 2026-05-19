#pragma once

#include "probe_state.h"
#include "report_helpers.h"
#include "selector_table_builder.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace selector_lookup_tables {

inline int RunSelectorLookupTablesProbe() {
  SelectorLookupProbeRun run;

  CaptureStartupObservations(run.startup);

  StageManualSelectorRegistrationTable();
  run.manual_register_status = RegisterManualSelectorImage();
  CaptureAfterManualObservations(run.after_manual);

  run.dynamic_handle_stable_id = LookupSelectorStableId(kDynamicSelector);
  CaptureAfterDynamicObservations(run.after_dynamic);

  objc3_runtime_reset_for_testing();
  CaptureAfterResetObservations(run.after_reset);

  run.replay_status = objc3_runtime_replay_registered_images_for_testing();
  CaptureAfterReplayObservations(run.after_replay);

  run.replayed_dynamic_handle_stable_id =
      LookupSelectorStableId(kDynamicSelector);
  CaptureAfterReplayDynamicObservations(run.after_replay_dynamic);

  PrintSelectorLookupTablesReport(run);
  return 0;
}

}  // namespace selector_lookup_tables
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
