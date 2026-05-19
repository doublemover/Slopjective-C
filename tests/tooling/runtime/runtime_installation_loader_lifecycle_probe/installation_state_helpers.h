#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_INSTALLATION_STATE_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_INSTALLATION_STATE_HELPERS_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime::installation_loader_lifecycle_probe {

inline std::string CopyRuntimeString(const char *value) {
  return value != nullptr ? value : "";
}

inline std::uint64_t DuplicateRegistrationOrdinal(
    const objc3_runtime_registration_state_snapshot &registration) {
  return registration.last_successful_registration_order_ordinal == 0
             ? 1u
             : registration.last_successful_registration_order_ordinal;
}

struct RegistrationState {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string rejected_module_name;
  std::string rejected_translation_unit_identity_key;
};

struct ImageWalkState {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
  std::string module_name;
  std::string translation_unit_identity_key;
};

struct ResetReplayState {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int copy_status = 0;
  std::string replayed_module_name;
  std::string replayed_translation_unit_identity_key;
};

struct StartupState {
  RegistrationState registration;
  ImageWalkState walk;
  ResetReplayState reset_replay;
};

struct PostResetState {
  RegistrationState registration;
  ResetReplayState reset_replay;
};

struct PostReplayState {
  RegistrationState registration;
  ImageWalkState walk;
  ResetReplayState reset_replay;
};

inline RegistrationState CaptureRegistrationState() {
  RegistrationState state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  state.module_name = CopyRuntimeString(state.snapshot.last_registered_module_name);
  state.translation_unit_identity_key =
      CopyRuntimeString(
          state.snapshot.last_registered_translation_unit_identity_key);
  state.rejected_module_name =
      CopyRuntimeString(state.snapshot.last_rejected_module_name);
  state.rejected_translation_unit_identity_key =
      CopyRuntimeString(
          state.snapshot.last_rejected_translation_unit_identity_key);
  return state;
}

inline ImageWalkState CaptureImageWalkState() {
  ImageWalkState state;
  state.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.snapshot);
  state.module_name = CopyRuntimeString(state.snapshot.last_walked_module_name);
  state.translation_unit_identity_key =
      CopyRuntimeString(state.snapshot.last_walked_translation_unit_identity_key);
  return state;
}

inline ResetReplayState CaptureResetReplayState() {
  ResetReplayState state;
  state.copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.snapshot);
  state.replayed_module_name =
      CopyRuntimeString(state.snapshot.last_replayed_module_name);
  state.replayed_translation_unit_identity_key =
      CopyRuntimeString(state.snapshot.last_replayed_translation_unit_identity_key);
  return state;
}

inline StartupState CaptureStartupState() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

inline PostResetState CapturePostResetState() {
  return {
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

inline PostReplayState CapturePostReplayState() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

}  // namespace objc3c::runtime::installation_loader_lifecycle_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_INSTALLATION_STATE_HELPERS_H_
