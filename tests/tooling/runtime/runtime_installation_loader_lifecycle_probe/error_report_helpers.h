#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_ERROR_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_ERROR_REPORT_HELPERS_H_

#include "installation_state_helpers.h"
#include "loader_lifecycle_assertions.h"
#include "../support/json_probe_writer.h"

#include <cstdint>
#include <cstdio>

namespace objc3c::runtime::installation_loader_lifecycle_probe {

inline unsigned long long JsonU64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintStartupReportFields(const StartupState &startup) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("startup_registration_copy_status",
                startup.registration.copy_status);
  PrintIntField("startup_image_walk_copy_status", startup.walk.copy_status);
  PrintIntField("startup_reset_replay_copy_status",
                startup.reset_replay.copy_status);
  PrintUint64Field("startup_registered_image_count",
                   JsonU64(startup.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "startup_next_expected_registration_order_ordinal",
      JsonU64(startup.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintUint64Field("startup_walked_image_count",
                   JsonU64(startup.walk.snapshot.walked_image_count));
  PrintUint64Field(
      "startup_last_discovery_root_entry_count",
      JsonU64(startup.walk.snapshot.last_discovery_root_entry_count));
  PrintIntField("startup_last_registration_used_staged_table",
                startup.walk.snapshot.last_registration_used_staged_table);
  PrintUint64Field(
      "startup_retained_bootstrap_image_count",
      JsonU64(startup.reset_replay.snapshot.retained_bootstrap_image_count));
  PrintStringField("startup_last_registered_module_name",
                   startup.registration.snapshot.last_registered_module_name);
  PrintStringField(
      "startup_last_registered_translation_unit_identity_key",
      startup.registration.snapshot
          .last_registered_translation_unit_identity_key);
}

inline void PrintDuplicateReportFields(
    const RegistrationAttemptResult &duplicate) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("duplicate_status", duplicate.status);
  PrintIntField("after_duplicate_registration_copy_status",
                duplicate.registration.copy_status);
  PrintIntField("after_duplicate_image_walk_copy_status",
                duplicate.walk.copy_status);
  PrintUint64Field(
      "after_duplicate_registered_image_count",
      JsonU64(duplicate.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "after_duplicate_next_expected_registration_order_ordinal",
      JsonU64(duplicate.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintUint64Field(
      "after_duplicate_last_successful_registration_order_ordinal",
      JsonU64(duplicate.registration.snapshot
                  .last_successful_registration_order_ordinal));
  PrintIntField("after_duplicate_last_registration_status",
                duplicate.registration.snapshot.last_registration_status);
  PrintStringField("after_duplicate_last_rejected_module_name",
                   duplicate.registration.rejected_module_name.c_str());
  PrintStringField(
      "after_duplicate_last_rejected_translation_unit_identity_key",
      duplicate.registration.rejected_translation_unit_identity_key.c_str());
  PrintUint64Field(
      "after_duplicate_last_rejected_registration_order_ordinal",
      JsonU64(duplicate.registration.snapshot
                  .last_rejected_registration_order_ordinal));
  PrintUint64Field("after_duplicate_walked_image_count",
                   JsonU64(duplicate.walk.snapshot.walked_image_count));
}

inline void PrintOutOfOrderReportFields(
    const RegistrationAttemptResult &out_of_order) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("out_of_order_status", out_of_order.status);
  PrintIntField("after_out_of_order_registration_copy_status",
                out_of_order.registration.copy_status);
  PrintIntField("after_out_of_order_image_walk_copy_status",
                out_of_order.walk.copy_status);
  PrintUint64Field(
      "after_out_of_order_registered_image_count",
      JsonU64(out_of_order.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "after_out_of_order_next_expected_registration_order_ordinal",
      JsonU64(out_of_order.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintUint64Field(
      "after_out_of_order_last_successful_registration_order_ordinal",
      JsonU64(out_of_order.registration.snapshot
                  .last_successful_registration_order_ordinal));
  PrintIntField("after_out_of_order_last_registration_status",
                out_of_order.registration.snapshot.last_registration_status);
  PrintStringField("after_out_of_order_last_rejected_module_name",
                   out_of_order.registration.rejected_module_name.c_str());
  PrintStringField(
      "after_out_of_order_last_rejected_translation_unit_identity_key",
      out_of_order.registration.rejected_translation_unit_identity_key.c_str());
  PrintUint64Field(
      "after_out_of_order_last_rejected_registration_order_ordinal",
      JsonU64(out_of_order.registration.snapshot
                  .last_rejected_registration_order_ordinal));
  PrintUint64Field("after_out_of_order_walked_image_count",
                   JsonU64(out_of_order.walk.snapshot.walked_image_count));
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
  PrintUint64Field(
      "post_reset_next_expected_registration_order_ordinal",
      JsonU64(post_reset.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintUint64Field(
      "post_reset_retained_bootstrap_image_count",
      JsonU64(post_reset.reset_replay.snapshot.retained_bootstrap_image_count));
  PrintUint64Field(
      "post_reset_last_reset_cleared_image_local_init_state_count",
      JsonU64(post_reset.reset_replay.snapshot
                  .last_reset_cleared_image_local_init_state_count));
}

inline void PrintInvalidAnchorReportFields(
    const RegistrationAttemptResult &invalid_anchor) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("post_reset_invalid_anchor_status", invalid_anchor.status);
  PrintIntField("after_invalid_anchor_registration_copy_status",
                invalid_anchor.registration.copy_status);
  PrintIntField("after_invalid_anchor_image_walk_copy_status",
                invalid_anchor.walk.copy_status);
  PrintUint64Field(
      "after_invalid_anchor_registered_image_count",
      JsonU64(invalid_anchor.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "after_invalid_anchor_next_expected_registration_order_ordinal",
      JsonU64(invalid_anchor.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintIntField("after_invalid_anchor_last_registration_status",
                invalid_anchor.registration.snapshot.last_registration_status);
  PrintUint64Field("after_invalid_anchor_walked_image_count",
                   JsonU64(invalid_anchor.walk.snapshot.walked_image_count));
  PrintIntField(
      "after_invalid_anchor_last_linker_anchor_matches_discovery_root",
      invalid_anchor.walk.snapshot.last_linker_anchor_matches_discovery_root);
}

inline void PrintInvalidDiscoveryRootReportFields(
    const RegistrationAttemptResult &invalid_discovery_root) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("post_reset_invalid_discovery_root_status",
                invalid_discovery_root.status);
  PrintIntField("after_invalid_discovery_root_registration_copy_status",
                invalid_discovery_root.registration.copy_status);
  PrintIntField("after_invalid_discovery_root_image_walk_copy_status",
                invalid_discovery_root.walk.copy_status);
  PrintUint64Field(
      "after_invalid_discovery_root_registered_image_count",
      JsonU64(
          invalid_discovery_root.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "after_invalid_discovery_root_next_expected_registration_order_ordinal",
      JsonU64(invalid_discovery_root.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintIntField(
      "after_invalid_discovery_root_last_registration_status",
      invalid_discovery_root.registration.snapshot.last_registration_status);
  PrintUint64Field(
      "after_invalid_discovery_root_walked_image_count",
      JsonU64(invalid_discovery_root.walk.snapshot.walked_image_count));
  PrintIntField(
      "after_invalid_discovery_root_last_linker_anchor_matches_discovery_root",
      invalid_discovery_root.walk.snapshot
          .last_linker_anchor_matches_discovery_root);
}

inline void PrintPostReplayReportFields(int replay_status,
                                        const PostReplayState &post_replay) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("replay_status", replay_status);
  PrintIntField("post_replay_registration_copy_status",
                post_replay.registration.copy_status);
  PrintIntField("post_replay_image_walk_copy_status",
                post_replay.walk.copy_status);
  PrintIntField("post_replay_reset_replay_copy_status",
                post_replay.reset_replay.copy_status);
  PrintUint64Field(
      "post_replay_registered_image_count",
      JsonU64(post_replay.registration.snapshot.registered_image_count));
  PrintUint64Field(
      "post_replay_next_expected_registration_order_ordinal",
      JsonU64(post_replay.registration.snapshot
                  .next_expected_registration_order_ordinal));
  PrintUint64Field("post_replay_walked_image_count",
                   JsonU64(post_replay.walk.snapshot.walked_image_count));
  PrintUint64Field(
      "post_replay_last_discovery_root_entry_count",
      JsonU64(post_replay.walk.snapshot.last_discovery_root_entry_count));
  PrintIntField("post_replay_last_registration_used_staged_table",
                post_replay.walk.snapshot.last_registration_used_staged_table);
  PrintUint64Field(
      "post_replay_retained_bootstrap_image_count",
      JsonU64(post_replay.reset_replay.snapshot.retained_bootstrap_image_count));
  PrintUint64Field(
      "post_replay_last_replayed_image_count",
      JsonU64(post_replay.reset_replay.snapshot.last_replayed_image_count));
  PrintUint64Field("post_replay_replay_generation",
                   JsonU64(post_replay.reset_replay.snapshot.replay_generation));
  PrintIntField("post_replay_last_replay_status",
                post_replay.reset_replay.snapshot.last_replay_status);
  PrintStringField("post_replay_last_registered_module_name",
                   post_replay.registration.module_name.c_str());
  PrintStringField("post_replay_last_walked_module_name",
                   post_replay.walk.module_name.c_str());
  PrintStringField("post_replay_last_replayed_module_name",
                   post_replay.reset_replay.replayed_module_name.c_str());
  PrintStringField(
      "post_replay_last_registered_translation_unit_identity_key",
      post_replay.registration.translation_unit_identity_key.c_str());
  PrintStringField(
      "post_replay_last_walked_translation_unit_identity_key",
      post_replay.walk.translation_unit_identity_key.c_str());
  PrintStringField(
      "post_replay_last_replayed_translation_unit_identity_key",
      post_replay.reset_replay.replayed_translation_unit_identity_key.c_str(),
      false);
}

inline void PrintProbeReport(
    const StartupState &startup,
    const RegistrationAttemptResult &duplicate,
    const RegistrationAttemptResult &out_of_order,
    const PostResetState &post_reset,
    const RegistrationAttemptResult &invalid_anchor,
    const RegistrationAttemptResult &invalid_discovery_root,
    int replay_status,
    const PostReplayState &post_replay) {
  std::printf("{");
  PrintStartupReportFields(startup);
  PrintDuplicateReportFields(duplicate);
  PrintOutOfOrderReportFields(out_of_order);
  PrintPostResetReportFields(post_reset);
  PrintInvalidAnchorReportFields(invalid_anchor);
  PrintInvalidDiscoveryRootReportFields(invalid_discovery_root);
  PrintPostReplayReportFields(replay_status, post_replay);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::installation_loader_lifecycle_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTALLATION_LOADER_LIFECYCLE_PROBE_ERROR_REPORT_HELPERS_H_
