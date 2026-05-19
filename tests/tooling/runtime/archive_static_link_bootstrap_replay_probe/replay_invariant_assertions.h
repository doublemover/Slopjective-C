#pragma once

#include "probe_result.h"

namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay {

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

}  // namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay
