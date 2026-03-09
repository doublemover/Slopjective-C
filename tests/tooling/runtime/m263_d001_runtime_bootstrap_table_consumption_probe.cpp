#include "runtime/objc3_runtime.h"
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
      case '\"':
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
  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_walk);

  const std::string startup_module_name =
      startup_registration.last_registered_module_name != nullptr
          ? startup_registration.last_registered_module_name
          : "";
  const std::string startup_identity_key =
      startup_registration.last_registered_translation_unit_identity_key != nullptr
          ? startup_registration.last_registered_translation_unit_identity_key
          : "";
  const std::uint64_t duplicate_ordinal =
      startup_registration.last_successful_registration_order_ordinal == 0
          ? 1u
          : startup_registration.last_successful_registration_order_ordinal;

  const objc3_runtime_image_descriptor duplicate_image = {
      startup_module_name.empty() ? "invalid" : startup_module_name.c_str(),
      startup_identity_key.empty() ? "invalid" : startup_identity_key.c_str(),
      duplicate_ordinal,
      startup_walk.last_walked_class_descriptor_count,
      startup_walk.last_walked_protocol_descriptor_count,
      startup_walk.last_walked_category_descriptor_count,
      startup_walk.last_walked_property_descriptor_count,
      startup_walk.last_walked_ivar_descriptor_count,
  };
  const int duplicate_registration_status =
      objc3_runtime_register_image(&duplicate_image);

  objc3_runtime_registration_state_snapshot after_duplicate_registration{};
  objc3_runtime_image_walk_state_snapshot after_duplicate_walk{};
  const int after_duplicate_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_duplicate_registration);
  const int after_duplicate_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_duplicate_walk);

  std::printf("{");
  std::printf("\"startup_registration_copy_status\":%d,",
              startup_registration_copy_status);
  std::printf("\"startup_image_walk_copy_status\":%d,",
              startup_image_walk_copy_status);
  std::printf("\"startup_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.registered_image_count));
  std::printf("\"startup_registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.registered_descriptor_total));
  std::printf("\"startup_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(startup_registration
                                                  .next_expected_registration_order_ordinal));
  std::printf("\"startup_last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(startup_registration
                                                  .last_successful_registration_order_ordinal));
  std::printf("\"startup_last_registration_status\":%d,",
              startup_registration.last_registration_status);
  std::printf("\"startup_last_registered_module_name\":");
  PrintJsonStringOrNull(startup_registration.last_registered_module_name);
  std::printf(",\"startup_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      startup_registration.last_registered_translation_unit_identity_key);
  std::printf(",\"startup_walked_image_count\":%llu,",
              static_cast<unsigned long long>(startup_walk.walked_image_count));
  std::printf(
      "\"startup_last_discovery_root_entry_count\":%llu,",
      static_cast<unsigned long long>(
          startup_walk.last_discovery_root_entry_count));
  std::printf(
      "\"startup_last_walked_class_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          startup_walk.last_walked_class_descriptor_count));
  std::printf(
      "\"startup_last_walked_protocol_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          startup_walk.last_walked_protocol_descriptor_count));
  std::printf(
      "\"startup_last_walked_category_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          startup_walk.last_walked_category_descriptor_count));
  std::printf(
      "\"startup_last_walked_property_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          startup_walk.last_walked_property_descriptor_count));
  std::printf(
      "\"startup_last_walked_ivar_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          startup_walk.last_walked_ivar_descriptor_count));
  std::printf(
      "\"startup_last_linker_anchor_matches_discovery_root\":%d,",
      startup_walk.last_linker_anchor_matches_discovery_root);
  std::printf("\"startup_last_registration_used_staged_table\":%d,",
              startup_walk.last_registration_used_staged_table);
  std::printf("\"duplicate_registration_status\":%d,",
              duplicate_registration_status);
  std::printf("\"after_duplicate_registration_copy_status\":%d,",
              after_duplicate_registration_copy_status);
  std::printf("\"after_duplicate_image_walk_copy_status\":%d,",
              after_duplicate_image_walk_copy_status);
  std::printf("\"after_duplicate_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_duplicate_registration.registered_image_count));
  std::printf("\"after_duplicate_registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(
                  after_duplicate_registration.registered_descriptor_total));
  std::printf(
      "\"after_duplicate_next_expected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(after_duplicate_registration
                                          .next_expected_registration_order_ordinal));
  std::printf(
      "\"after_duplicate_last_successful_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(after_duplicate_registration
                                          .last_successful_registration_order_ordinal));
  std::printf("\"after_duplicate_last_registration_status\":%d,",
              after_duplicate_registration.last_registration_status);
  std::printf("\"after_duplicate_last_rejected_module_name\":");
  PrintJsonStringOrNull(after_duplicate_registration.last_rejected_module_name);
  std::printf(",\"after_duplicate_last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      after_duplicate_registration.last_rejected_translation_unit_identity_key);
  std::printf(
      ",\"after_duplicate_last_rejected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(after_duplicate_registration
                                          .last_rejected_registration_order_ordinal));
  std::printf(
      "\"after_duplicate_walked_image_count\":%llu,",
      static_cast<unsigned long long>(after_duplicate_walk.walked_image_count));
  std::printf(
      "\"after_duplicate_last_walked_class_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          after_duplicate_walk.last_walked_class_descriptor_count));
  std::printf(
      "\"after_duplicate_last_walked_protocol_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          after_duplicate_walk.last_walked_protocol_descriptor_count));
  std::printf(
      "\"after_duplicate_last_walked_category_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          after_duplicate_walk.last_walked_category_descriptor_count));
  std::printf(
      "\"after_duplicate_last_walked_property_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          after_duplicate_walk.last_walked_property_descriptor_count));
  std::printf(
      "\"after_duplicate_last_walked_ivar_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          after_duplicate_walk.last_walked_ivar_descriptor_count));
  std::printf(
      "\"after_duplicate_last_linker_anchor_matches_discovery_root\":%d,",
      after_duplicate_walk.last_linker_anchor_matches_discovery_root);
  std::printf(
      "\"after_duplicate_last_registration_used_staged_table\":%d",
      after_duplicate_walk.last_registration_used_staged_table);
  std::printf("}\n");
  return 0;
}
