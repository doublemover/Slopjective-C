#pragma once

#include "replay_tracking_snapshots.h"

namespace objc3c::runtime::probe::live_registration_replay_tracking {

struct ReplayTrackingCycleResult {
  PostResetReplayTrackingSnapshot post_reset;
  int replay_status = 0;
  PostReplayTrackingSnapshot post_replay;
};

struct ProbeResult {
  StartupReplayTrackingSnapshot startup;
  ReplayTrackingCycleResult replay;
};

inline ReplayTrackingCycleResult RunResetReplayTrackingCycle() {
  ResetRuntimeRegistrationFixture();

  ReplayTrackingCycleResult result;
  result.post_reset = CapturePostResetReplayTrackingSnapshot();
  result.replay_status = ReplayRegisteredImagesForTracking();
  result.post_replay = CapturePostReplayTrackingSnapshot();
  return result;
}

inline const RegistrationStateObservation &StartupRegistration(
    const ProbeResult &result) {
  return result.startup.registration;
}

inline const ImageWalkStateObservation &StartupImageWalk(
    const ProbeResult &result) {
  return result.startup.walk;
}

inline const ResetReplayStateObservation &StartupResetReplay(
    const ProbeResult &result) {
  return result.startup.replay;
}

inline const RegistrationStateObservation &PostResetRegistration(
    const ProbeResult &result) {
  return result.replay.post_reset.registration;
}

inline const ResetReplayStateObservation &PostResetReplay(
    const ProbeResult &result) {
  return result.replay.post_reset.replay;
}

inline const RegistrationStateObservation &PostReplayRegistration(
    const ProbeResult &result) {
  return result.replay.post_replay.registration;
}

inline const ImageWalkStateObservation &PostReplayImageWalk(
    const ProbeResult &result) {
  return result.replay.post_replay.walk;
}

inline const ResetReplayStateObservation &PostReplayResetReplay(
    const ProbeResult &result) {
  return result.replay.post_replay.replay;
}

}  // namespace objc3c::runtime::probe::live_registration_replay_tracking
