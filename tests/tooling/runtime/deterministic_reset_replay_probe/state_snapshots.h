#pragma once

#include "fixture_setup.h"

namespace objc3c::runtime::probe::deterministic_reset_replay {

struct RegistrationState {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ImageWalkState {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ResetReplayState {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int copy_status = 0;
};

struct StartupState {
  RegistrationState registration;
  ImageWalkState image_walk;
  ResetReplayState reset_replay;
  std::uint64_t known_selector_stable_id = 0;
};

struct PostResetState {
  RegistrationState registration;
  ResetReplayState reset_replay;
};

struct PostReplayState {
  RegistrationState registration;
  ImageWalkState image_walk;
  ResetReplayState reset_replay;
  std::uint64_t known_selector_stable_id = 0;
  std::uint64_t unknown_selector_stable_id = 0;
};

inline RegistrationState CaptureRegistrationState() {
  RegistrationState state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  return state;
}

inline ImageWalkState CaptureImageWalkState() {
  ImageWalkState state;
  state.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.snapshot);
  return state;
}

inline ResetReplayState CaptureResetReplayState() {
  ResetReplayState state;
  state.copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.snapshot);
  return state;
}

inline StartupState CaptureStartupState() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
      LookupSelectorStableId(kKnownSelectorName),
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
      LookupSelectorStableId(kKnownSelectorName),
      LookupSelectorStableId(kUnknownSelectorName),
  };
}

}  // namespace objc3c::runtime::probe::deterministic_reset_replay
