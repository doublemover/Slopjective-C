#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_PACKAGE_MODULE_FIXTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_PACKAGE_MODULE_FIXTURE_H_

#include "cross_module_runtime_packaging_probe/probe_result.h"

namespace objc3c::runtime::probe::cross_module_runtime_packaging {

inline void StabilizeStartupPackageModuleState(
    StartupPackageModuleState &state) {
  state.last_registered_translation_unit_identity_key = CopyRuntimeCString(
      state.registration.last_registered_translation_unit_identity_key);
}

inline StartupPackageModuleState CaptureStartupPackageModuleState() {
  StartupPackageModuleState state;
  state.registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.registration);
  state.imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kImportedProviderClassName, &state.imported_entry);
  state.local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kLocalConsumerClassName, &state.local_entry);
  StabilizeStartupPackageModuleState(state);
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

inline void StabilizePostReplayPackageModuleState(
    PostReplayPackageModuleState &state) {
  state.last_registered_translation_unit_identity_key = CopyRuntimeCString(
      state.registration.last_registered_translation_unit_identity_key);
  state.last_replayed_translation_unit_identity_key = CopyRuntimeCString(
      state.replay.last_replayed_translation_unit_identity_key);
}

inline PostReplayPackageModuleState CapturePostReplayPackageModuleState() {
  PostReplayPackageModuleState state;
  state.registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.registration);
  state.replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.replay);
  state.imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kImportedProviderClassName, &state.imported_entry);
  state.local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          kLocalConsumerClassName, &state.local_entry);
  StabilizePostReplayPackageModuleState(state);
  return state;
}

}  // namespace objc3c::runtime::probe::cross_module_runtime_packaging

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CROSS_MODULE_RUNTIME_PACKAGING_PROBE_PACKAGE_MODULE_FIXTURE_H_
