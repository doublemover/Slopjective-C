#pragma once

#include "../support/json_probe_writer.h"
#include "replay_assertion_helpers.h"

#include <cstdio>

namespace objc3c::runtime::probe::live_registration_replay_tracking {

template <typename Value> inline unsigned long long JsonU64(Value value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintStartupFields(const ProbeResult &result) {
  const RegistrationStateObservation &registration =
      StartupRegistration(result);
  const ImageWalkStateObservation &walk = StartupImageWalk(result);
  const ResetReplayStateObservation &replay = StartupResetReplay(result);

  PrintIntField("startup_registration_copy_status", registration.copy_status);
  PrintIntField("startup_image_walk_copy_status", walk.copy_status);
  PrintIntField("startup_reset_replay_copy_status", replay.copy_status);
  PrintUint64Field("startup_registered_image_count",
                   JsonU64(registration.snapshot.registered_image_count));
  PrintUint64Field(
      "startup_next_expected_registration_order_ordinal",
      JsonU64(registration.snapshot.next_expected_registration_order_ordinal));
  PrintUint64Field("startup_walked_image_count",
                   JsonU64(walk.snapshot.walked_image_count));
  PrintUint64Field("startup_last_discovery_root_entry_count",
                   JsonU64(walk.snapshot.last_discovery_root_entry_count));
  PrintIntField("startup_last_registration_used_staged_table",
                walk.snapshot.last_registration_used_staged_table);
  PrintUint64Field("startup_retained_bootstrap_image_count",
                   JsonU64(replay.snapshot.retained_bootstrap_image_count));
  PrintStringField("startup_last_registered_module_name",
                   registration.snapshot.last_registered_module_name);
  PrintStringField(
      "startup_last_registered_translation_unit_identity_key",
      registration.snapshot.last_registered_translation_unit_identity_key);
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
      "post_reset_live_registration_order_entry_count",
      JsonU64(replay.snapshot.live_registration_order_entry_count));
  PrintUint64Field(
      "post_reset_live_registered_metadata_entry_count",
      JsonU64(replay.snapshot.live_registered_metadata_entry_count));
  PrintUint64Field("post_reset_live_selector_table_entry_count",
                   JsonU64(replay.snapshot.live_selector_table_entry_count));
  PrintUint64Field("post_reset_live_keypath_entry_count",
                   JsonU64(replay.snapshot.live_keypath_entry_count));
  PrintUint64Field("post_reset_live_realized_class_count",
                   JsonU64(replay.snapshot.live_realized_class_count));
  PrintUint64Field("post_reset_live_method_cache_entry_count",
                   JsonU64(replay.snapshot.live_method_cache_entry_count));
  PrintUint64Field(
      "post_reset_live_property_lookup_cache_entry_count",
      JsonU64(replay.snapshot.live_property_lookup_cache_entry_count));
  PrintUint64Field(
      "post_reset_last_reset_cleared_image_local_init_state_count",
      JsonU64(replay.snapshot.last_reset_cleared_image_local_init_state_count));
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
  PrintUint64Field(
      "post_replay_next_expected_registration_order_ordinal",
      JsonU64(registration.snapshot.next_expected_registration_order_ordinal));
  PrintUint64Field("post_replay_walked_image_count",
                   JsonU64(walk.snapshot.walked_image_count));
  PrintUint64Field("post_replay_last_discovery_root_entry_count",
                   JsonU64(walk.snapshot.last_discovery_root_entry_count));
  PrintIntField("post_replay_last_registration_used_staged_table",
                walk.snapshot.last_registration_used_staged_table);
  PrintUint64Field("post_replay_retained_bootstrap_image_count",
                   JsonU64(replay.snapshot.retained_bootstrap_image_count));
  PrintUint64Field("post_replay_last_replayed_image_count",
                   JsonU64(replay.snapshot.last_replayed_image_count));
  PrintUint64Field("post_replay_replay_generation",
                   JsonU64(replay.snapshot.replay_generation));
  PrintIntField("post_replay_last_replay_status",
                replay.snapshot.last_replay_status);
  PrintStringField("post_replay_last_registered_module_name",
                   registration.snapshot.last_registered_module_name);
  PrintStringField("post_replay_last_walked_module_name",
                   walk.snapshot.last_walked_module_name);
  PrintStringField("post_replay_last_replayed_module_name",
                   replay.snapshot.last_replayed_module_name);
  PrintStringField(
      "post_replay_last_registered_translation_unit_identity_key",
      registration.snapshot.last_registered_translation_unit_identity_key);
  PrintStringField("post_replay_last_walked_translation_unit_identity_key",
                   walk.snapshot.last_walked_translation_unit_identity_key);
  PrintStringField("post_replay_last_replayed_translation_unit_identity_key",
                   replay.snapshot.last_replayed_translation_unit_identity_key,
                   false);
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintStartupFields(result);
  PrintPostResetFields(result);
  PrintPostReplayFields(result);
  std::printf("}\n");
}

} // namespace objc3c::runtime::probe::live_registration_replay_tracking
