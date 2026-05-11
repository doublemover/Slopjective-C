#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_FIXTURE_MODULE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_FIXTURE_MODULE_SETUP_H_

#include "import_module_execution_matrix_probe/probe_result.h"

namespace objc3c::runtime::probe::import_module_execution_matrix {

inline void StabilizeStartupFixtureModuleState(
    StartupFixtureModuleState &state) {
  state.imported_module_name =
      CopyRuntimeCString(state.imported_entry.module_name);
  state.imported_translation_unit_identity_key =
      CopyRuntimeCString(state.imported_entry.translation_unit_identity_key);
  state.imported_class_owner_identity =
      CopyRuntimeCString(state.imported_entry.class_owner_identity);
  state.local_module_name = CopyRuntimeCString(state.local_entry.module_name);
  state.local_translation_unit_identity_key =
      CopyRuntimeCString(state.local_entry.translation_unit_identity_key);
  state.local_class_owner_identity =
      CopyRuntimeCString(state.local_entry.class_owner_identity);
}

inline StartupFixtureModuleState CaptureStartupFixtureModuleState() {
  StartupFixtureModuleState state;
  state.registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.registration);
  state.image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.image_walk);
  state.graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(&state.graph);
  state.imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kImportedProviderClassName, &state.imported_entry);
  state.local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kLocalConsumerClassName, &state.local_entry);
  return state;
}

inline PostResetRuntimeState CapturePostResetRuntimeState() {
  PostResetRuntimeState state;
  state.registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.registration);
  state.replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.replay);
  return state;
}

inline PostReplayFixtureModuleState CapturePostReplayFixtureModuleState() {
  PostReplayFixtureModuleState state;
  state.registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.registration);
  state.image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.image_walk);
  state.graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(&state.graph);
  state.replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.replay);
  state.imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kImportedProviderClassName, &state.imported_entry);
  state.local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kLocalConsumerClassName, &state.local_entry);
  return state;
}

}  // namespace objc3c::runtime::probe::import_module_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_FIXTURE_MODULE_SETUP_H_
