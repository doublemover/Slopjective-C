#pragma once

#include "registration_fixture_setup.h"

namespace objc3c::runtime::probe::live_registration_replay_tracking {

struct RegistrationStateObservation {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ImageWalkStateObservation {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ResetReplayStateObservation {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int copy_status = 0;
};

struct StartupReplayTrackingSnapshot {
  RegistrationStateObservation registration;
  ImageWalkStateObservation walk;
  ResetReplayStateObservation replay;
};

struct PostResetReplayTrackingSnapshot {
  RegistrationStateObservation registration;
  ResetReplayStateObservation replay;
};

struct PostReplayTrackingSnapshot {
  RegistrationStateObservation registration;
  ImageWalkStateObservation walk;
  ResetReplayStateObservation replay;
};

inline RegistrationStateObservation CaptureRegistrationState() {
  RegistrationStateObservation observation;
  observation.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&observation.snapshot);
  return observation;
}

inline ImageWalkStateObservation CaptureImageWalkState() {
  ImageWalkStateObservation observation;
  observation.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&observation.snapshot);
  return observation;
}

inline ResetReplayStateObservation CaptureResetReplayState() {
  ResetReplayStateObservation observation;
  observation.copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&observation.snapshot);
  return observation;
}

inline StartupReplayTrackingSnapshot CaptureStartupReplayTrackingSnapshot() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

inline PostResetReplayTrackingSnapshot CapturePostResetReplayTrackingSnapshot() {
  return {
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

inline PostReplayTrackingSnapshot CapturePostReplayTrackingSnapshot() {
  return {
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

}  // namespace objc3c::runtime::probe::live_registration_replay_tracking
