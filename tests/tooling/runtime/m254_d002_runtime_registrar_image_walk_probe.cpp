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
  objc3_runtime_registration_state_snapshot registration_snapshot{};
  objc3_runtime_image_walk_state_snapshot image_walk_snapshot{};
  const int registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&registration_snapshot);
  const int image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&image_walk_snapshot);
  const objc3_runtime_selector_handle *known_selector =
      objc3_runtime_lookup_selector("tokenValue");
  const objc3_runtime_selector_handle *unknown_selector =
      objc3_runtime_lookup_selector("__objc3_unknown_probe_selector");
  const std::string last_registered_module_name =
      registration_snapshot.last_registered_module_name != nullptr
          ? registration_snapshot.last_registered_module_name
          : "";
  const std::string last_registered_translation_unit_identity_key =
      registration_snapshot.last_registered_translation_unit_identity_key != nullptr
          ? registration_snapshot.last_registered_translation_unit_identity_key
          : "";
  const std::string last_walked_module_name =
      image_walk_snapshot.last_walked_module_name != nullptr
          ? image_walk_snapshot.last_walked_module_name
          : "";
  const std::string last_walked_translation_unit_identity_key =
      image_walk_snapshot.last_walked_translation_unit_identity_key != nullptr
          ? image_walk_snapshot.last_walked_translation_unit_identity_key
          : "";

  std::printf("{");
  std::printf("\"registration_copy_status\":%d,", registration_copy_status);
  std::printf("\"image_walk_copy_status\":%d,", image_walk_copy_status);
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  registration_snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(
                  registration_snapshot.registered_descriptor_total));
  std::printf("\"last_registration_status\":%d,",
              registration_snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(last_registered_module_name.c_str());
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      last_registered_translation_unit_identity_key.c_str());
  std::printf(",\"walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  image_walk_snapshot.walked_image_count));
  std::printf(
      "\"last_discovery_root_entry_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_discovery_root_entry_count));
  std::printf(
      "\"last_walked_class_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_class_descriptor_count));
  std::printf(
      "\"last_walked_protocol_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_protocol_descriptor_count));
  std::printf(
      "\"last_walked_category_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_category_descriptor_count));
  std::printf(
      "\"last_walked_property_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_property_descriptor_count));
  std::printf(
      "\"last_walked_ivar_descriptor_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_ivar_descriptor_count));
  std::printf(
      "\"last_walked_selector_pool_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_selector_pool_count));
  std::printf(
      "\"last_walked_string_pool_count\":%llu,",
      static_cast<unsigned long long>(
          image_walk_snapshot.last_walked_string_pool_count));
  std::printf(
      "\"last_linker_anchor_matches_discovery_root\":%d,",
      image_walk_snapshot.last_linker_anchor_matches_discovery_root);
  std::printf(
      "\"last_registration_used_staged_table\":%d,",
      image_walk_snapshot.last_registration_used_staged_table);
  std::printf("\"last_walked_module_name\":");
  PrintJsonStringOrNull(last_walked_module_name.c_str());
  std::printf(",\"last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(last_walked_translation_unit_identity_key.c_str());
  std::printf(",\"known_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(
                  known_selector != nullptr ? known_selector->stable_id : 0));
  std::printf(
      "\"unknown_selector_stable_id\":%llu",
      static_cast<unsigned long long>(
          unknown_selector != nullptr ? unknown_selector->stable_id : 0));
  std::printf("}\n");
  return 0;
}
