#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <cstdio>

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
  objc3_runtime_reset_replay_state_snapshot startup_reset_replay{};
  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&startup_reset_replay);

  const int unsupported_replay_status =
      objc3_runtime_replay_registered_images_for_testing();
  objc3_runtime_registration_state_snapshot unsupported_replay_registration{};
  objc3_runtime_reset_replay_state_snapshot unsupported_replay_reset_replay{};
  const int unsupported_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &unsupported_replay_registration);
  const int unsupported_replay_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &unsupported_replay_reset_replay);

  objc3_runtime_reset_for_testing();
  objc3_runtime_registration_state_snapshot post_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot post_reset_reset_replay{};
  const int post_reset_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_reset_registration);
  const int post_reset_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_reset_reset_replay);

  const int first_restart_status = objc3_runtime_replay_registered_images_for_testing();
  objc3_runtime_registration_state_snapshot first_restart_registration{};
  objc3_runtime_image_walk_state_snapshot first_restart_image_walk{};
  objc3_runtime_reset_replay_state_snapshot first_restart_reset_replay{};
  const int first_restart_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&first_restart_registration);
  const int first_restart_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&first_restart_image_walk);
  const int first_restart_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &first_restart_reset_replay);

  const int second_unsupported_replay_status =
      objc3_runtime_replay_registered_images_for_testing();
  objc3_runtime_registration_state_snapshot second_unsupported_replay_registration{};
  objc3_runtime_reset_replay_state_snapshot second_unsupported_replay_reset_replay{};
  const int second_unsupported_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &second_unsupported_replay_registration);
  const int second_unsupported_replay_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &second_unsupported_replay_reset_replay);

  objc3_runtime_reset_for_testing();
  objc3_runtime_registration_state_snapshot second_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot second_reset_reset_replay{};
  const int second_reset_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&second_reset_registration);
  const int second_reset_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &second_reset_reset_replay);

  const int second_restart_status = objc3_runtime_replay_registered_images_for_testing();
  objc3_runtime_registration_state_snapshot second_restart_registration{};
  objc3_runtime_image_walk_state_snapshot second_restart_image_walk{};
  objc3_runtime_reset_replay_state_snapshot second_restart_reset_replay{};
  const int second_restart_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&second_restart_registration);
  const int second_restart_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&second_restart_image_walk);
  const int second_restart_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &second_restart_reset_replay);

  std::printf("{");
  std::printf("\"startup_registration_copy_status\":%d,", startup_registration_copy_status);
  std::printf("\"startup_reset_replay_copy_status\":%d,", startup_reset_replay_copy_status);
  std::printf("\"startup_registered_image_count\":%llu,",
              static_cast<unsigned long long>(startup_registration.registered_image_count));
  std::printf("\"startup_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(startup_registration.last_registered_translation_unit_identity_key);

  std::printf(",\"unsupported_replay_status\":%d,", unsupported_replay_status);
  std::printf("\"unsupported_replay_registration_copy_status\":%d,", unsupported_replay_registration_copy_status);
  std::printf("\"unsupported_replay_reset_replay_copy_status\":%d,", unsupported_replay_reset_replay_copy_status);
  std::printf("\"unsupported_replay_registered_image_count\":%llu,",
              static_cast<unsigned long long>(unsupported_replay_registration.registered_image_count));
  std::printf("\"unsupported_replay_last_replay_status\":%d,",
              unsupported_replay_reset_replay.last_replay_status);
  std::printf("\"unsupported_replay_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(unsupported_replay_reset_replay.last_replayed_image_count));

  std::printf("\"post_reset_registration_copy_status\":%d,", post_reset_registration_copy_status);
  std::printf("\"post_reset_reset_replay_copy_status\":%d,", post_reset_reset_replay_copy_status);
  std::printf("\"post_reset_registered_image_count\":%llu,",
              static_cast<unsigned long long>(post_reset_registration.registered_image_count));
  std::printf("\"post_reset_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(post_reset_registration.next_expected_registration_order_ordinal));
  std::printf("\"post_reset_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(post_reset_reset_replay.retained_bootstrap_image_count));
  std::printf("\"post_reset_last_reset_cleared_image_local_init_state_count\":%llu,",
              static_cast<unsigned long long>(post_reset_reset_replay.last_reset_cleared_image_local_init_state_count));
  std::printf("\"post_reset_reset_generation\":%llu,",
              static_cast<unsigned long long>(post_reset_reset_replay.reset_generation));

  std::printf("\"first_restart_status\":%d,", first_restart_status);
  std::printf("\"first_restart_registration_copy_status\":%d,", first_restart_registration_copy_status);
  std::printf("\"first_restart_image_walk_copy_status\":%d,", first_restart_image_walk_copy_status);
  std::printf("\"first_restart_reset_replay_copy_status\":%d,", first_restart_reset_replay_copy_status);
  std::printf("\"first_restart_registered_image_count\":%llu,",
              static_cast<unsigned long long>(first_restart_registration.registered_image_count));
  std::printf("\"first_restart_last_registration_status\":%d,",
              first_restart_registration.last_registration_status);
  std::printf("\"first_restart_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(first_restart_reset_replay.last_replayed_image_count));
  std::printf("\"first_restart_replay_generation\":%llu,",
              static_cast<unsigned long long>(first_restart_reset_replay.replay_generation));
  std::printf("\"first_restart_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(first_restart_registration.last_registered_translation_unit_identity_key);
  std::printf(",\"first_restart_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(first_restart_reset_replay.last_replayed_translation_unit_identity_key);
  std::printf(",\"first_restart_last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(first_restart_image_walk.last_walked_translation_unit_identity_key);

  std::printf(",\"second_unsupported_replay_status\":%d,", second_unsupported_replay_status);
  std::printf("\"second_unsupported_replay_registration_copy_status\":%d,", second_unsupported_replay_registration_copy_status);
  std::printf("\"second_unsupported_replay_reset_replay_copy_status\":%d,", second_unsupported_replay_reset_replay_copy_status);
  std::printf("\"second_unsupported_replay_registered_image_count\":%llu,",
              static_cast<unsigned long long>(second_unsupported_replay_registration.registered_image_count));
  std::printf("\"second_unsupported_replay_last_replay_status\":%d,",
              second_unsupported_replay_reset_replay.last_replay_status);

  std::printf("\"second_reset_registration_copy_status\":%d,", second_reset_registration_copy_status);
  std::printf("\"second_reset_reset_replay_copy_status\":%d,", second_reset_reset_replay_copy_status);
  std::printf("\"second_reset_registered_image_count\":%llu,",
              static_cast<unsigned long long>(second_reset_registration.registered_image_count));
  std::printf("\"second_reset_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(second_reset_registration.next_expected_registration_order_ordinal));
  std::printf("\"second_reset_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(second_reset_reset_replay.retained_bootstrap_image_count));
  std::printf("\"second_reset_last_reset_cleared_image_local_init_state_count\":%llu,",
              static_cast<unsigned long long>(second_reset_reset_replay.last_reset_cleared_image_local_init_state_count));
  std::printf("\"second_reset_reset_generation\":%llu,",
              static_cast<unsigned long long>(second_reset_reset_replay.reset_generation));

  std::printf("\"second_restart_status\":%d,", second_restart_status);
  std::printf("\"second_restart_registration_copy_status\":%d,", second_restart_registration_copy_status);
  std::printf("\"second_restart_image_walk_copy_status\":%d,", second_restart_image_walk_copy_status);
  std::printf("\"second_restart_reset_replay_copy_status\":%d,", second_restart_reset_replay_copy_status);
  std::printf("\"second_restart_registered_image_count\":%llu,",
              static_cast<unsigned long long>(second_restart_registration.registered_image_count));
  std::printf("\"second_restart_last_registration_status\":%d,",
              second_restart_registration.last_registration_status);
  std::printf("\"second_restart_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(second_restart_reset_replay.last_replayed_image_count));
  std::printf("\"second_restart_replay_generation\":%llu,",
              static_cast<unsigned long long>(second_restart_reset_replay.replay_generation));
  std::printf("\"second_restart_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(second_restart_registration.last_registered_translation_unit_identity_key);
  std::printf(",\"second_restart_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(second_restart_reset_replay.last_replayed_translation_unit_identity_key);
  std::printf(",\"second_restart_last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(second_restart_image_walk.last_walked_translation_unit_identity_key);

  std::printf("}\n");
  return 0;
}
