#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay {

struct RegistrationStateObservation {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_registered_translation_unit_identity_key;
};

struct ImageWalkStateObservation {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_walked_translation_unit_identity_key;
};

struct ResetReplayStateObservation {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int copy_status = 0;
  std::string last_replayed_translation_unit_identity_key;
};

struct ArchiveStaticLinkFixtureState {
  RegistrationStateObservation registration;
  ImageWalkStateObservation walk;
  ResetReplayStateObservation replay;
};

struct PostResetBootstrapReplayState {
  RegistrationStateObservation registration;
  ResetReplayStateObservation replay;
};

struct PostReplayBootstrapReplayState {
  RegistrationStateObservation registration;
  ImageWalkStateObservation walk;
  ResetReplayStateObservation replay;
};

struct BootstrapReplayScenarioResult {
  PostResetBootstrapReplayState post_reset;
  int replay_status = 0;
  PostReplayBootstrapReplayState post_replay;
};

struct ProbeResult {
  ArchiveStaticLinkFixtureState startup;
  BootstrapReplayScenarioResult replay;
};

}  // namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay
