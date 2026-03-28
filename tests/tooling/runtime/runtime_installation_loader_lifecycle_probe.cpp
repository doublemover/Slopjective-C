#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

namespace {

void PrintJsonStringOrNull(const char *value) {
  if (value == nullptr) {
    std::printf("null");
    return;
  }
  std::printf("\"");
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(value);
       *cursor != 0U; ++cursor) {
    switch (*cursor) {
      case '\\':
        std::printf("\\\\");
        break;
      case '"':
        std::printf("\\\"");
        break;
      case '\n':
        std::printf("\\n");
        break;
      case '\r':
        std::printf("\\r");
        break;
      case '\t':
        std::printf("\\t");
        break;
      default:
        std::printf("%c", static_cast<char>(*cursor));
        break;
    }
  }
  std::printf("\"");
}

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot startup_registration{};
  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const std::string startup_module_name =
      startup_registration.last_registered_module_name != nullptr
          ? startup_registration.last_registered_module_name
          : "";
  const std::string startup_identity_key =
      startup_registration.last_registered_translation_unit_identity_key != nullptr
          ? startup_registration.last_registered_translation_unit_identity_key
          : "";
  objc3_runtime_image_walk_state_snapshot startup_walk{};
  const int startup_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_walk);
  objc3_runtime_reset_replay_state_snapshot startup_reset_replay{};
  const int startup_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&startup_reset_replay);

  const std::uint64_t startup_registration_ordinal =
      startup_registration.last_successful_registration_order_ordinal == 0
          ? 1u
          : startup_registration.last_successful_registration_order_ordinal;
  const objc3_runtime_image_descriptor duplicate_image{
      startup_module_name.empty() ? "invalid-module" : startup_module_name.c_str(),
      startup_identity_key.empty() ? "invalid-identity" : startup_identity_key.c_str(),
      startup_registration_ordinal,
      startup_walk.last_walked_class_descriptor_count,
      startup_walk.last_walked_protocol_descriptor_count,
      startup_walk.last_walked_category_descriptor_count,
      startup_walk.last_walked_property_descriptor_count,
      startup_walk.last_walked_ivar_descriptor_count,
  };
  const std::string out_of_order_identity =
      startup_identity_key.empty()
          ? "out-of-order-identity"
          : startup_identity_key + "-out-of-order";
  const objc3_runtime_image_descriptor out_of_order_image{
      "out-of-order-module",
      out_of_order_identity.c_str(),
      startup_registration.next_expected_registration_order_ordinal + 1u,
      startup_walk.last_walked_class_descriptor_count,
      startup_walk.last_walked_protocol_descriptor_count,
      startup_walk.last_walked_category_descriptor_count,
      startup_walk.last_walked_property_descriptor_count,
      startup_walk.last_walked_ivar_descriptor_count,
  };
  const int duplicate_status = objc3_runtime_register_image(&duplicate_image);
  objc3_runtime_registration_state_snapshot after_duplicate_registration{};
  objc3_runtime_image_walk_state_snapshot after_duplicate_walk{};
  const int after_duplicate_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_duplicate_registration);
  const std::string after_duplicate_last_rejected_module_name =
      after_duplicate_registration.last_rejected_module_name != nullptr
          ? after_duplicate_registration.last_rejected_module_name
          : "";
  const std::string after_duplicate_last_rejected_translation_unit_identity_key =
      after_duplicate_registration.last_rejected_translation_unit_identity_key !=
              nullptr
          ? after_duplicate_registration.last_rejected_translation_unit_identity_key
          : "";
  const int after_duplicate_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_duplicate_walk);
  const int out_of_order_status =
      objc3_runtime_register_image(&out_of_order_image);
  objc3_runtime_registration_state_snapshot after_out_of_order_registration{};
  objc3_runtime_image_walk_state_snapshot after_out_of_order_walk{};
  const int after_out_of_order_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_out_of_order_registration);
  const std::string after_out_of_order_last_rejected_module_name =
      after_out_of_order_registration.last_rejected_module_name != nullptr
          ? after_out_of_order_registration.last_rejected_module_name
          : "";
  const std::string after_out_of_order_last_rejected_translation_unit_identity_key =
      after_out_of_order_registration
                  .last_rejected_translation_unit_identity_key != nullptr
          ? after_out_of_order_registration
                .last_rejected_translation_unit_identity_key
          : "";
  const int after_out_of_order_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_out_of_order_walk);

  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot post_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot post_reset_reset_replay{};
  const int post_reset_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_reset_registration);
  const int post_reset_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_reset_reset_replay);

  const int replay_status = objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot post_replay_registration{};
  const int post_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_replay_registration);
  const std::string post_replay_last_registered_module_name =
      post_replay_registration.last_registered_module_name != nullptr
          ? post_replay_registration.last_registered_module_name
          : "";
  const std::string post_replay_last_registered_translation_unit_identity_key =
      post_replay_registration.last_registered_translation_unit_identity_key !=
              nullptr
          ? post_replay_registration.last_registered_translation_unit_identity_key
          : "";
  objc3_runtime_image_walk_state_snapshot post_replay_walk{};
  const int post_replay_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&post_replay_walk);
  const std::string post_replay_last_walked_module_name =
      post_replay_walk.last_walked_module_name != nullptr
          ? post_replay_walk.last_walked_module_name
          : "";
  const std::string post_replay_last_walked_translation_unit_identity_key =
      post_replay_walk.last_walked_translation_unit_identity_key != nullptr
          ? post_replay_walk.last_walked_translation_unit_identity_key
          : "";
  objc3_runtime_reset_replay_state_snapshot post_replay_reset_replay{};
  const int post_replay_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_replay_reset_replay);
  const std::string post_replay_last_replayed_module_name =
      post_replay_reset_replay.last_replayed_module_name != nullptr
          ? post_replay_reset_replay.last_replayed_module_name
          : "";
  const std::string post_replay_last_replayed_translation_unit_identity_key =
      post_replay_reset_replay.last_replayed_translation_unit_identity_key !=
              nullptr
          ? post_replay_reset_replay.last_replayed_translation_unit_identity_key
          : "";

  std::printf("{");
  std::printf("\"startup_registration_copy_status\":%d,",
              startup_registration_copy_status);
  std::printf("\"startup_image_walk_copy_status\":%d,",
              startup_image_walk_copy_status);
  std::printf("\"startup_reset_replay_copy_status\":%d,",
              startup_reset_replay_copy_status);
  std::printf("\"startup_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.registered_image_count));
  std::printf("\"startup_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.next_expected_registration_order_ordinal));
  std::printf("\"startup_walked_image_count\":%llu,",
              static_cast<unsigned long long>(startup_walk.walked_image_count));
  std::printf("\"startup_last_discovery_root_entry_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_walk.last_discovery_root_entry_count));
  std::printf("\"startup_last_registration_used_staged_table\":%d,",
              startup_walk.last_registration_used_staged_table);
  std::printf("\"startup_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_reset_replay.retained_bootstrap_image_count));
  std::printf("\"startup_last_registered_module_name\":");
  PrintJsonStringOrNull(startup_registration.last_registered_module_name);
  std::printf(",\"startup_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      startup_registration.last_registered_translation_unit_identity_key);
  std::printf(",\"duplicate_status\":%d,", duplicate_status);
  std::printf("\"after_duplicate_registration_copy_status\":%d,",
              after_duplicate_registration_copy_status);
  std::printf("\"after_duplicate_image_walk_copy_status\":%d,",
              after_duplicate_image_walk_copy_status);
  std::printf("\"after_duplicate_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_duplicate_registration.registered_image_count));
  std::printf("\"after_duplicate_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_duplicate_registration
                                                  .next_expected_registration_order_ordinal));
  std::printf("\"after_duplicate_last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_duplicate_registration
                                                  .last_successful_registration_order_ordinal));
  std::printf("\"after_duplicate_last_registration_status\":%d,",
              after_duplicate_registration.last_registration_status);
  std::printf("\"after_duplicate_last_rejected_module_name\":");
  PrintJsonStringOrNull(after_duplicate_last_rejected_module_name.c_str());
  std::printf(",\"after_duplicate_last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      after_duplicate_last_rejected_translation_unit_identity_key.c_str());
  std::printf(",\"after_duplicate_last_rejected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_duplicate_registration
                                                  .last_rejected_registration_order_ordinal));
  std::printf("\"after_duplicate_walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_duplicate_walk.walked_image_count));
  std::printf("\"out_of_order_status\":%d,", out_of_order_status);
  std::printf("\"after_out_of_order_registration_copy_status\":%d,",
              after_out_of_order_registration_copy_status);
  std::printf("\"after_out_of_order_image_walk_copy_status\":%d,",
              after_out_of_order_image_walk_copy_status);
  std::printf("\"after_out_of_order_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_out_of_order_registration.registered_image_count));
  std::printf("\"after_out_of_order_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_out_of_order_registration
                                                  .next_expected_registration_order_ordinal));
  std::printf("\"after_out_of_order_last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_out_of_order_registration
                                                  .last_successful_registration_order_ordinal));
  std::printf("\"after_out_of_order_last_registration_status\":%d,",
              after_out_of_order_registration.last_registration_status);
  std::printf("\"after_out_of_order_last_rejected_module_name\":");
  PrintJsonStringOrNull(after_out_of_order_last_rejected_module_name.c_str());
  std::printf(",\"after_out_of_order_last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      after_out_of_order_last_rejected_translation_unit_identity_key.c_str());
  std::printf(
      ",\"after_out_of_order_last_rejected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(after_out_of_order_registration
                                          .last_rejected_registration_order_ordinal));
  std::printf("\"after_out_of_order_walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_out_of_order_walk.walked_image_count));

  std::printf("\"post_reset_registration_copy_status\":%d,",
              post_reset_registration_copy_status);
  std::printf("\"post_reset_reset_replay_copy_status\":%d,",
              post_reset_reset_replay_copy_status);
  std::printf("\"post_reset_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_registration.registered_image_count));
  std::printf("\"post_reset_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_registration.next_expected_registration_order_ordinal));
  std::printf("\"post_reset_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_reset_replay.retained_bootstrap_image_count));
  std::printf("\"post_reset_last_reset_cleared_image_local_init_state_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_reset_replay
                      .last_reset_cleared_image_local_init_state_count));

  std::printf("\"replay_status\":%d,", replay_status);
  std::printf("\"post_replay_registration_copy_status\":%d,",
              post_replay_registration_copy_status);
  std::printf("\"post_replay_image_walk_copy_status\":%d,",
              post_replay_image_walk_copy_status);
  std::printf("\"post_replay_reset_replay_copy_status\":%d,",
              post_replay_reset_replay_copy_status);
  std::printf("\"post_replay_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration.registered_image_count));
  std::printf("\"post_replay_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration
                      .next_expected_registration_order_ordinal));
  std::printf("\"post_replay_walked_image_count\":%llu,",
              static_cast<unsigned long long>(post_replay_walk.walked_image_count));
  std::printf("\"post_replay_last_discovery_root_entry_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_walk.last_discovery_root_entry_count));
  std::printf("\"post_replay_last_registration_used_staged_table\":%d,",
              post_replay_walk.last_registration_used_staged_table);
  std::printf("\"post_replay_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.retained_bootstrap_image_count));
  std::printf("\"post_replay_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.last_replayed_image_count));
  std::printf("\"post_replay_replay_generation\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.replay_generation));
  std::printf("\"post_replay_last_replay_status\":%d,",
              post_replay_reset_replay.last_replay_status);
  std::printf("\"post_replay_last_registered_module_name\":");
  PrintJsonStringOrNull(post_replay_last_registered_module_name.c_str());
  std::printf(",\"post_replay_last_walked_module_name\":");
  PrintJsonStringOrNull(post_replay_last_walked_module_name.c_str());
  std::printf(",\"post_replay_last_replayed_module_name\":");
  PrintJsonStringOrNull(post_replay_last_replayed_module_name.c_str());
  std::printf(",\"post_replay_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_registered_translation_unit_identity_key.c_str());
  std::printf(",\"post_replay_last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_walked_translation_unit_identity_key.c_str());
  std::printf(",\"post_replay_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_replayed_translation_unit_identity_key.c_str());
  std::printf("}\n");
  return 0;
}
