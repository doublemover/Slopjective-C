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
  objc3_runtime_image_walk_state_snapshot startup_walk{};
  objc3_runtime_reset_replay_state_snapshot startup_reset_replay{};
  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_walk);
  const int startup_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&startup_reset_replay);
  const std::string startup_last_registered_translation_unit_identity_key =
      startup_registration.last_registered_translation_unit_identity_key != nullptr
          ? startup_registration.last_registered_translation_unit_identity_key
          : "";

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
  objc3_runtime_image_walk_state_snapshot post_replay_walk{};
  objc3_runtime_reset_replay_state_snapshot post_replay_reset_replay{};
  const int post_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_replay_registration);
  const int post_replay_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&post_replay_walk);
  const int post_replay_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_replay_reset_replay);
  const std::string post_replay_last_registered_translation_unit_identity_key =
      post_replay_registration.last_registered_translation_unit_identity_key != nullptr
          ? post_replay_registration.last_registered_translation_unit_identity_key
          : "";
  const std::string post_replay_last_walked_translation_unit_identity_key =
      post_replay_walk.last_walked_translation_unit_identity_key != nullptr
          ? post_replay_walk.last_walked_translation_unit_identity_key
          : "";
  const std::string post_replay_last_replayed_translation_unit_identity_key =
      post_replay_reset_replay.last_replayed_translation_unit_identity_key != nullptr
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
  std::printf("\"startup_walked_image_count\":%llu,",
              static_cast<unsigned long long>(startup_walk.walked_image_count));
  std::printf("\"startup_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.next_expected_registration_order_ordinal));
  std::printf("\"startup_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      startup_last_registered_translation_unit_identity_key.empty()
          ? nullptr
          : startup_last_registered_translation_unit_identity_key.c_str());

  std::printf(",\"post_reset_registration_copy_status\":%d,",
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
  std::printf("\"post_replay_walked_image_count\":%llu,",
              static_cast<unsigned long long>(post_replay_walk.walked_image_count));
  std::printf("\"post_replay_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration
                      .next_expected_registration_order_ordinal));
  std::printf("\"post_replay_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.last_replayed_image_count));
  std::printf("\"post_replay_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_registered_translation_unit_identity_key.empty()
          ? nullptr
          : post_replay_last_registered_translation_unit_identity_key.c_str());
  std::printf(",\"post_replay_last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_walked_translation_unit_identity_key.empty()
          ? nullptr
          : post_replay_last_walked_translation_unit_identity_key.c_str());
  std::printf(",\"post_replay_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_replayed_translation_unit_identity_key.empty()
          ? nullptr
          : post_replay_last_replayed_translation_unit_identity_key.c_str());
  std::printf("}\n");
  return 0;
}
