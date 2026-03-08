#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
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
  objc3_runtime_image_walk_state_snapshot startup_image_walk{};
  objc3_runtime_reset_replay_state_snapshot startup_reset_replay{};
  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_image_walk);
  const int startup_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&startup_reset_replay);
  const objc3_runtime_selector_handle *startup_known_selector =
      objc3_runtime_lookup_selector("tokenValue");
  const std::uint64_t startup_known_selector_stable_id =
      startup_known_selector != nullptr ? startup_known_selector->stable_id : 0;

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
  objc3_runtime_image_walk_state_snapshot post_replay_image_walk{};
  objc3_runtime_reset_replay_state_snapshot post_replay_reset_replay{};
  const int post_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_replay_registration);
  const int post_replay_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&post_replay_image_walk);
  const int post_replay_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_replay_reset_replay);
  const objc3_runtime_selector_handle *replay_known_selector =
      objc3_runtime_lookup_selector("tokenValue");
  const objc3_runtime_selector_handle *replay_unknown_selector =
      objc3_runtime_lookup_selector("__objc3_d003_unknown_selector");

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
              static_cast<unsigned long long>(startup_image_walk.walked_image_count));
  std::printf("\"startup_last_walked_selector_pool_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_image_walk.last_walked_selector_pool_count));
  std::printf("\"startup_known_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(startup_known_selector_stable_id));
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
  std::printf(
      "\"post_reset_last_reset_cleared_image_local_init_state_count\":%llu,",
      static_cast<unsigned long long>(
          post_reset_reset_replay.last_reset_cleared_image_local_init_state_count));
  std::printf("\"post_reset_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_reset_replay.retained_bootstrap_image_count));
  std::printf("\"post_reset_reset_generation\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_reset_replay.reset_generation));
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
              static_cast<unsigned long long>(
                  post_replay_image_walk.walked_image_count));
  std::printf("\"post_replay_last_registration_status\":%d,",
              post_replay_registration.last_registration_status);
  std::printf("\"post_replay_last_registration_used_staged_table\":%d,",
              post_replay_image_walk.last_registration_used_staged_table);
  std::printf("\"post_replay_last_walked_selector_pool_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_image_walk.last_walked_selector_pool_count));
  std::printf("\"post_replay_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_registration.last_registered_translation_unit_identity_key);
  std::printf(",\"post_replay_last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_image_walk.last_walked_translation_unit_identity_key);
  std::printf(",\"post_replay_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_reset_replay.last_replayed_translation_unit_identity_key);
  std::printf(",\"post_replay_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.last_replayed_image_count));
  std::printf("\"post_replay_replay_generation\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.replay_generation));
  std::printf("\"replay_known_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(
                  replay_known_selector != nullptr
                      ? replay_known_selector->stable_id
                      : 0));
  std::printf("\"replay_unknown_selector_stable_id\":%llu",
              static_cast<unsigned long long>(
                  replay_unknown_selector != nullptr
                      ? replay_unknown_selector->stable_id
                      : 0));
  std::printf("}\n");
  return 0;
}
