#pragma once

#include "probe_result.h"
#include "../support/json_probe_writer.h"

namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay {

inline std::string CopyRuntimeString(const char *value) {
  return objc3c::runtime::probe::CopyJsonString(value);
}

inline const char *NullableRuntimeString(const std::string &value) {
  return objc3c::runtime::probe::NullableCString(value);
}

inline RegistrationStateObservation CaptureRegistrationState() {
  RegistrationStateObservation state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  state.last_registered_translation_unit_identity_key = CopyRuntimeString(
      state.snapshot.last_registered_translation_unit_identity_key);
  return state;
}

inline ImageWalkStateObservation CaptureImageWalkState() {
  ImageWalkStateObservation state;
  state.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.snapshot);
  state.last_walked_translation_unit_identity_key = CopyRuntimeString(
      state.snapshot.last_walked_translation_unit_identity_key);
  return state;
}

inline ResetReplayStateObservation CaptureResetReplayState() {
  ResetReplayStateObservation state;
  state.copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.snapshot);
  state.last_replayed_translation_unit_identity_key = CopyRuntimeString(
      state.snapshot.last_replayed_translation_unit_identity_key);
  return state;
}

inline ArchiveStaticLinkFixtureState CaptureStartupArchiveStaticLinkFixture() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

inline PostResetBootstrapReplayState CapturePostResetBootstrapReplayState() {
  return {
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

inline PostReplayBootstrapReplayState CapturePostReplayBootstrapReplayState() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

}  // namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay
