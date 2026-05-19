#pragma once

#include "archive_static_link_fixture_setup.h"
#include "replay_invariant_assertions.h"
#include "../support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay {

template <typename Value>
inline unsigned long long JsonU64(Value value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintStartupFields(const ProbeResult &result) {
  const RegistrationStateObservation &registration =
      StartupRegistration(result);
  const ImageWalkStateObservation &walk = StartupImageWalk(result);
  const ResetReplayStateObservation &replay = StartupResetReplay(result);

  PrintIntField("startup_registration_copy_status",
                registration.copy_status);
  PrintIntField("startup_image_walk_copy_status", walk.copy_status);
  PrintIntField("startup_reset_replay_copy_status", replay.copy_status);
  PrintUint64Field("startup_registered_image_count",
                   JsonU64(registration.snapshot.registered_image_count));
  PrintUint64Field("startup_walked_image_count",
                   JsonU64(walk.snapshot.walked_image_count));
  PrintUint64Field(
      "startup_next_expected_registration_order_ordinal",
      JsonU64(registration.snapshot.next_expected_registration_order_ordinal));
  PrintStringField(
      "startup_last_registered_translation_unit_identity_key",
      NullableRuntimeString(
          registration.last_registered_translation_unit_identity_key));
}

inline void PrintPostResetFields(const ProbeResult &result) {
  const RegistrationStateObservation &registration =
      PostResetRegistration(result);
  const ResetReplayStateObservation &replay = PostResetReplay(result);

  PrintIntField("post_reset_registration_copy_status",
                registration.copy_status);
  PrintIntField("post_reset_reset_replay_copy_status", replay.copy_status);
  PrintUint64Field("post_reset_registered_image_count",
                   JsonU64(registration.snapshot.registered_image_count));
  PrintUint64Field(
      "post_reset_next_expected_registration_order_ordinal",
      JsonU64(registration.snapshot.next_expected_registration_order_ordinal));
  PrintUint64Field("post_reset_retained_bootstrap_image_count",
                   JsonU64(replay.snapshot.retained_bootstrap_image_count));
  PrintUint64Field(
      "post_reset_last_reset_cleared_image_local_init_state_count",
      JsonU64(
          replay.snapshot.last_reset_cleared_image_local_init_state_count));
}

inline void PrintPostReplayFields(const ProbeResult &result) {
  const RegistrationStateObservation &registration =
      PostReplayRegistration(result);
  const ImageWalkStateObservation &walk = PostReplayImageWalk(result);
  const ResetReplayStateObservation &replay = PostReplayResetReplay(result);

  PrintIntField("replay_status", result.replay.replay_status);
  PrintIntField("post_replay_registration_copy_status",
                registration.copy_status);
  PrintIntField("post_replay_image_walk_copy_status", walk.copy_status);
  PrintIntField("post_replay_reset_replay_copy_status", replay.copy_status);
  PrintUint64Field("post_replay_registered_image_count",
                   JsonU64(registration.snapshot.registered_image_count));
  PrintUint64Field("post_replay_walked_image_count",
                   JsonU64(walk.snapshot.walked_image_count));
  PrintUint64Field(
      "post_replay_next_expected_registration_order_ordinal",
      JsonU64(registration.snapshot.next_expected_registration_order_ordinal));
  PrintUint64Field("post_replay_last_replayed_image_count",
                   JsonU64(replay.snapshot.last_replayed_image_count));
  PrintStringField(
      "post_replay_last_registered_translation_unit_identity_key",
      NullableRuntimeString(
          registration.last_registered_translation_unit_identity_key));
  PrintStringField(
      "post_replay_last_walked_translation_unit_identity_key",
      NullableRuntimeString(walk.last_walked_translation_unit_identity_key));
  PrintStringField(
      "post_replay_last_replayed_translation_unit_identity_key",
      NullableRuntimeString(replay.last_replayed_translation_unit_identity_key),
      false);
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintStartupFields(result);
  PrintPostResetFields(result);
  PrintPostReplayFields(result);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay
