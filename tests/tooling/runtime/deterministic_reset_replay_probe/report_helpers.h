#pragma once

#include "../support/json_probe_writer.h"
#include "probe_result.h"

#include <cstdint>
#include <cstdio>

namespace objc3c::runtime::probe::deterministic_reset_replay {

inline unsigned long long JsonU64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintStartupReportFields(const StartupState &startup) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("startup_registration_copy_status",
                startup.registration.copy_status);
  PrintIntField("startup_image_walk_copy_status",
                startup.image_walk.copy_status);
  PrintIntField("startup_reset_replay_copy_status",
                startup.reset_replay.copy_status);
  PrintUint64Field(
      "startup_registered_image_count",
      JsonU64(startup.registration.snapshot.registered_image_count));
  PrintUint64Field("startup_walked_image_count",
                   JsonU64(startup.image_walk.snapshot.walked_image_count));
  PrintUint64Field(
      "startup_last_walked_selector_pool_count",
      JsonU64(startup.image_walk.snapshot.last_walked_selector_pool_count));
  PrintUint64Field("startup_known_selector_stable_id",
                   JsonU64(startup.known_selector_stable_id));
}

inline void PrintPostResetReportFields(const PostResetState &post_reset) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("post_reset_registration_copy_status",
                post_reset.registration.copy_status);
  PrintIntField("post_reset_reset_replay_copy_status",
                post_reset.reset_replay.copy_status);
  PrintUint64Field(
      "post_reset_registered_image_count",
      JsonU64(post_reset.registration.snapshot.registered_image_count));
  PrintUint64Field("post_reset_next_expected_registration_order_ordinal",
                   JsonU64(post_reset.registration.snapshot
                               .next_expected_registration_order_ordinal));
  PrintUint64Field(
      "post_reset_last_reset_cleared_image_local_init_state_count",
      JsonU64(post_reset.reset_replay.snapshot
                  .last_reset_cleared_image_local_init_state_count));
  PrintUint64Field(
      "post_reset_retained_bootstrap_image_count",
      JsonU64(post_reset.reset_replay.snapshot.retained_bootstrap_image_count));
  PrintUint64Field("post_reset_live_registration_order_entry_count",
                   JsonU64(post_reset.reset_replay.snapshot
                               .live_registration_order_entry_count));
  PrintUint64Field("post_reset_live_registered_metadata_entry_count",
                   JsonU64(post_reset.reset_replay.snapshot
                               .live_registered_metadata_entry_count));
  PrintUint64Field(
      "post_reset_live_selector_table_entry_count",
      JsonU64(
          post_reset.reset_replay.snapshot.live_selector_table_entry_count));
  PrintUint64Field(
      "post_reset_live_keypath_entry_count",
      JsonU64(post_reset.reset_replay.snapshot.live_keypath_entry_count));
  PrintUint64Field(
      "post_reset_live_realized_class_count",
      JsonU64(post_reset.reset_replay.snapshot.live_realized_class_count));
  PrintUint64Field(
      "post_reset_live_method_cache_entry_count",
      JsonU64(post_reset.reset_replay.snapshot.live_method_cache_entry_count));
  PrintUint64Field("post_reset_live_property_lookup_cache_entry_count",
                   JsonU64(post_reset.reset_replay.snapshot
                               .live_property_lookup_cache_entry_count));
  PrintUint64Field("post_reset_reset_generation",
                   JsonU64(post_reset.reset_replay.snapshot.reset_generation));
}

inline void
PrintPostReplayReportFields(const ResetReplayCycleResult &reset_replay) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  const PostReplayState &post_replay = reset_replay.post_replay;

  PrintIntField("replay_status", reset_replay.replay_status);
  PrintIntField("post_replay_registration_copy_status",
                post_replay.registration.copy_status);
  PrintIntField("post_replay_image_walk_copy_status",
                post_replay.image_walk.copy_status);
  PrintIntField("post_replay_reset_replay_copy_status",
                post_replay.reset_replay.copy_status);
  PrintUint64Field(
      "post_replay_registered_image_count",
      JsonU64(post_replay.registration.snapshot.registered_image_count));
  PrintUint64Field("post_replay_walked_image_count",
                   JsonU64(post_replay.image_walk.snapshot.walked_image_count));
  PrintIntField("post_replay_last_registration_status",
                post_replay.registration.snapshot.last_registration_status);
  PrintIntField(
      "post_replay_last_registration_used_staged_table",
      post_replay.image_walk.snapshot.last_registration_used_staged_table);
  PrintUint64Field(
      "post_replay_last_walked_selector_pool_count",
      JsonU64(post_replay.image_walk.snapshot.last_walked_selector_pool_count));
  PrintStringField("post_replay_last_registered_translation_unit_identity_key",
                   post_replay.registration.snapshot
                       .last_registered_translation_unit_identity_key);
  PrintStringField("post_replay_last_walked_translation_unit_identity_key",
                   post_replay.image_walk.snapshot
                       .last_walked_translation_unit_identity_key);
  PrintStringField("post_replay_last_replayed_translation_unit_identity_key",
                   post_replay.reset_replay.snapshot
                       .last_replayed_translation_unit_identity_key);
  PrintUint64Field(
      "post_replay_last_replayed_image_count",
      JsonU64(post_replay.reset_replay.snapshot.last_replayed_image_count));
  PrintUint64Field(
      "post_replay_replay_generation",
      JsonU64(post_replay.reset_replay.snapshot.replay_generation));
  PrintUint64Field("replay_known_selector_stable_id",
                   JsonU64(post_replay.known_selector_stable_id));
  PrintUint64Field("replay_unknown_selector_stable_id",
                   JsonU64(post_replay.unknown_selector_stable_id), false);
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintStartupReportFields(result.startup);
  PrintPostResetReportFields(result.reset_replay.post_reset);
  PrintPostReplayReportFields(result.reset_replay);
  std::printf("}\n");
}

} // namespace objc3c::runtime::probe::deterministic_reset_replay
