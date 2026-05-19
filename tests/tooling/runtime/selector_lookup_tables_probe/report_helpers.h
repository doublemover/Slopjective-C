#pragma once

#include "probe_state.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c {
namespace runtime {
namespace probe {
namespace selector_lookup_tables {

inline void PrintImageWalkState(
    const objc3_runtime_image_walk_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"walked_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.walked_image_count));
  std::printf("\"last_walked_selector_pool_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_walked_selector_pool_count));
  std::printf("\"last_registration_used_staged_table\":%d,",
              snapshot.last_registration_used_staged_table);
  std::printf("\"last_walked_translation_unit_identity_key\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.last_walked_translation_unit_identity_key);
  std::printf("}");
}

inline void PrintResetReplayState(
    const objc3_runtime_reset_replay_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.retained_bootstrap_image_count));
  std::printf("\"last_reset_cleared_image_local_init_state_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_reset_cleared_image_local_init_state_count));
  std::printf("\"last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_replayed_image_count));
  std::printf("\"reset_generation\":%llu,",
              static_cast<unsigned long long>(snapshot.reset_generation));
  std::printf("\"replay_generation\":%llu,",
              static_cast<unsigned long long>(snapshot.replay_generation));
  std::printf("\"last_replay_status\":%d,", snapshot.last_replay_status);
  std::printf("\"last_replayed_translation_unit_identity_key\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.last_replayed_translation_unit_identity_key);
  std::printf("}");
}

inline void PrintSelectorLookupStatusFields(
    const SelectorLookupProbeRun &run) {
  std::printf("\"startup_registration_status\":%d,",
              run.startup.registration.status);
  std::printf("\"startup_image_walk_status\":%d,",
              run.startup.image_walk.status);
  std::printf("\"startup_table_status\":%d,", run.startup.table.status);
  std::printf("\"startup_token_status\":%d,", run.startup.token.status);
  std::printf("\"startup_current_status\":%d,", run.startup.current.status);
  std::printf("\"startup_set_current_status\":%d,",
              run.startup.set_current.status);
  std::printf("\"startup_shared_status\":%d,", run.startup.shared.status);
  std::printf("\"startup_manual_only_before_status\":%d,",
              run.startup.manual_only_before.status);
  std::printf("\"manual_register_status\":%d,", run.manual_register_status);
  std::printf("\"after_manual_registration_status\":%d,",
              run.after_manual.registration.status);
  std::printf("\"after_manual_image_walk_status\":%d,",
              run.after_manual.image_walk.status);
  std::printf("\"after_manual_table_status\":%d,",
              run.after_manual.table.status);
  std::printf("\"after_manual_token_status\":%d,",
              run.after_manual.token.status);
  std::printf("\"after_manual_debug_name_status\":%d,",
              run.after_manual.debug_name.status);
  std::printf("\"after_dynamic_table_status\":%d,",
              run.after_dynamic.table.status);
  std::printf("\"after_dynamic_manual_only_status\":%d,",
              run.after_dynamic.manual_only.status);
  std::printf("\"after_reset_registration_status\":%d,",
              run.after_reset.registration.status);
  std::printf("\"after_reset_table_status\":%d,", run.after_reset.table.status);
  std::printf("\"replay_status\":%d,", run.replay_status);
  std::printf("\"after_replay_registration_status\":%d,",
              run.after_replay.registration.status);
  std::printf("\"after_replay_image_walk_status\":%d,",
              run.after_replay.image_walk.status);
  std::printf("\"after_replay_reset_replay_status\":%d,",
              run.after_replay.reset_replay.status);
  std::printf("\"after_replay_table_status\":%d,",
              run.after_replay.table.status);
  std::printf("\"after_replay_token_status\":%d,",
              run.after_replay.token.status);
  std::printf("\"after_replay_debug_name_status\":%d,",
              run.after_replay.debug_name.status);
  std::printf("\"after_replay_manual_only_before_status\":%d,",
              run.after_replay.manual_only_before.status);
  std::printf("\"after_replay_dynamic_table_status\":%d,",
              run.after_replay_dynamic.table.status);
  std::printf("\"after_replay_dynamic_manual_only_status\":%d,",
              run.after_replay_dynamic.manual_only.status);
  std::printf("\"dynamic_handle_stable_id\":%llu,",
              static_cast<unsigned long long>(run.dynamic_handle_stable_id));
  std::printf("\"replayed_dynamic_handle_stable_id\":%llu,",
              static_cast<unsigned long long>(
                  run.replayed_dynamic_handle_stable_id));
}

inline void PrintSelectorLookupSnapshotFields(
    const SelectorLookupProbeRun &run) {
  std::printf("\"startup_registration\":");
  ::objc3c::runtime::probe::PrintRegistrationStateSelectorLookup(
      run.startup.registration.snapshot);
  std::printf(",\"startup_image_walk\":");
  PrintImageWalkState(run.startup.image_walk.snapshot);
  std::printf(",\"startup_table\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.startup.table.snapshot);
  std::printf(",\"startup_token\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.startup.token.snapshot);
  std::printf(",\"startup_current\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.startup.current.snapshot);
  std::printf(",\"startup_set_current\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.startup.set_current.snapshot);
  std::printf(",\"startup_shared\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.startup.shared.snapshot);
  std::printf(",\"startup_manual_only_before\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.startup.manual_only_before.snapshot);

  std::printf(",\"after_manual_registration\":");
  ::objc3c::runtime::probe::PrintRegistrationStateSelectorLookup(
      run.after_manual.registration.snapshot);
  std::printf(",\"after_manual_image_walk\":");
  PrintImageWalkState(run.after_manual.image_walk.snapshot);
  std::printf(",\"after_manual_table\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.after_manual.table.snapshot);
  std::printf(",\"after_manual_token\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_manual.token.snapshot);
  std::printf(",\"after_manual_debug_name\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_manual.debug_name.snapshot);

  std::printf(",\"after_dynamic_table\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.after_dynamic.table.snapshot);
  std::printf(",\"after_dynamic_manual_only\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_dynamic.manual_only.snapshot);

  std::printf(",\"after_reset_registration\":");
  ::objc3c::runtime::probe::PrintRegistrationStateSelectorLookup(
      run.after_reset.registration.snapshot);
  std::printf(",\"after_reset_table\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.after_reset.table.snapshot);

  std::printf(",\"after_replay_registration\":");
  ::objc3c::runtime::probe::PrintRegistrationStateSelectorLookup(
      run.after_replay.registration.snapshot);
  std::printf(",\"after_replay_image_walk\":");
  PrintImageWalkState(run.after_replay.image_walk.snapshot);
  std::printf(",\"after_replay_reset_replay\":");
  PrintResetReplayState(run.after_replay.reset_replay.snapshot);
  std::printf(",\"after_replay_table\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.after_replay.table.snapshot);
  std::printf(",\"after_replay_token\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_replay.token.snapshot);
  std::printf(",\"after_replay_debug_name\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_replay.debug_name.snapshot);
  std::printf(",\"after_replay_manual_only_before\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_replay.manual_only_before.snapshot);
  std::printf(",\"after_replay_dynamic_table\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.after_replay_dynamic.table.snapshot);
  std::printf(",\"after_replay_dynamic_manual_only\":");
  ::objc3c::runtime::probe::PrintSelectorEntryFull(
      run.after_replay_dynamic.manual_only.snapshot);
}

inline void PrintSelectorLookupTablesReport(
    const SelectorLookupProbeRun &run) {
  std::printf("{");
  PrintSelectorLookupStatusFields(run);
  PrintSelectorLookupSnapshotFields(run);
  std::printf("}\n");
}

}  // namespace selector_lookup_tables
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
