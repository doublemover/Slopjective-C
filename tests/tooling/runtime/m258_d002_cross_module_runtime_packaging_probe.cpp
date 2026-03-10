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

  objc3_runtime_realized_class_entry_snapshot imported_entry{};
  objc3_runtime_realized_class_entry_snapshot local_entry{};
  const int imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing("ImportedProvider",
                                                          &imported_entry);
  const int local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing("LocalConsumer",
                                                          &local_entry);

  const int imported_class_receiver =
      imported_entry.found != 0
          ? static_cast<int>(imported_entry.base_identity + 2U)
          : 0;
  const int local_class_receiver =
      local_entry.found != 0 ? static_cast<int>(local_entry.base_identity + 2U)
                             : 0;

  const int imported_provider_class_value =
      objc3_runtime_dispatch_i32(imported_class_receiver, "providerClassValue",
                                 0, 0, 0, 0);
  const int imported_provider_protocol_value =
      objc3_runtime_dispatch_i32(imported_class_receiver,
                                 "importedProtocolValue", 0, 0, 0, 0);
  const int local_consumer_class_value =
      objc3_runtime_dispatch_i32(local_class_receiver, "localClassValue", 0, 0,
                                 0, 0);
  objc3_runtime_protocol_conformance_query_snapshot imported_worker_query{};
  const int imported_worker_query_status =
      objc3_runtime_copy_protocol_conformance_query_for_testing(
          "ImportedProvider", "ImportedWorker", &imported_worker_query);

  objc3_runtime_reset_for_testing();
  objc3_runtime_registration_state_snapshot post_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot post_reset_replay{};
  const int post_reset_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_reset_registration);
  const int post_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&post_reset_replay);

  const int replay_status = objc3_runtime_replay_registered_images_for_testing();
  objc3_runtime_registration_state_snapshot post_replay_registration{};
  objc3_runtime_reset_replay_state_snapshot post_replay_replay{};
  objc3_runtime_realized_class_entry_snapshot post_replay_imported_entry{};
  objc3_runtime_realized_class_entry_snapshot post_replay_local_entry{};
  const int post_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &post_replay_registration);
  const int post_replay_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&post_replay_replay);
  const int post_replay_imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "ImportedProvider", &post_replay_imported_entry);
  const int post_replay_local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "LocalConsumer", &post_replay_local_entry);

  const int post_replay_imported_class_receiver =
      post_replay_imported_entry.found != 0
          ? static_cast<int>(post_replay_imported_entry.base_identity + 2U)
          : 0;
  const int post_replay_local_class_receiver =
      post_replay_local_entry.found != 0
          ? static_cast<int>(post_replay_local_entry.base_identity + 2U)
          : 0;
  const int post_replay_imported_provider_class_value =
      objc3_runtime_dispatch_i32(post_replay_imported_class_receiver,
                                 "providerClassValue", 0, 0, 0, 0);
  const int post_replay_imported_provider_protocol_value =
      objc3_runtime_dispatch_i32(post_replay_imported_class_receiver,
                                 "importedProtocolValue", 0, 0, 0, 0);
  const int post_replay_local_consumer_class_value =
      objc3_runtime_dispatch_i32(post_replay_local_class_receiver,
                                 "localClassValue", 0, 0, 0, 0);

  const std::string startup_last_registered_identity =
      startup_registration.last_registered_translation_unit_identity_key != nullptr
          ? startup_registration.last_registered_translation_unit_identity_key
          : "";
  const std::string post_replay_last_registered_identity =
      post_replay_registration.last_registered_translation_unit_identity_key != nullptr
          ? post_replay_registration.last_registered_translation_unit_identity_key
          : "";
  const std::string post_replay_last_replayed_identity =
      post_replay_replay.last_replayed_translation_unit_identity_key != nullptr
          ? post_replay_replay.last_replayed_translation_unit_identity_key
          : "";

  std::printf("{");
  std::printf("\"startup_registration_copy_status\":%d,",
              startup_registration_copy_status);
  std::printf("\"startup_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.registered_image_count));
  std::printf("\"startup_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.next_expected_registration_order_ordinal));
  std::printf("\"startup_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(startup_last_registered_identity.empty()
                            ? nullptr
                            : startup_last_registered_identity.c_str());
  std::printf(",\"imported_entry_status\":%d,", imported_entry_status);
  std::printf("\"local_entry_status\":%d,", local_entry_status);
  std::printf("\"imported_entry_found\":%d,", imported_entry.found);
  std::printf("\"local_entry_found\":%d,", local_entry.found);
  std::printf("\"imported_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  imported_entry.registration_order_ordinal));
  std::printf("\"local_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  local_entry.registration_order_ordinal));
  std::printf("\"imported_provider_class_value\":%d,",
              imported_provider_class_value);
  std::printf("\"imported_provider_protocol_value\":%d,",
              imported_provider_protocol_value);
  std::printf("\"local_consumer_class_value\":%d,",
              local_consumer_class_value);
  std::printf("\"imported_worker_query_status\":%d,",
              imported_worker_query_status);
  std::printf("\"imported_worker_conforms\":%d,",
              imported_worker_query.conforms);
  std::printf("\"post_reset_registration_copy_status\":%d,",
              post_reset_registration_copy_status);
  std::printf("\"post_reset_replay_copy_status\":%d,",
              post_reset_replay_copy_status);
  std::printf("\"post_reset_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_registration.registered_image_count));
  std::printf("\"post_reset_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_replay.retained_bootstrap_image_count));
  std::printf("\"replay_status\":%d,", replay_status);
  std::printf("\"post_replay_registration_copy_status\":%d,",
              post_replay_registration_copy_status);
  std::printf("\"post_replay_replay_copy_status\":%d,",
              post_replay_replay_copy_status);
  std::printf("\"post_replay_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration.registered_image_count));
  std::printf("\"post_replay_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration
                      .next_expected_registration_order_ordinal));
  std::printf("\"post_replay_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(post_replay_last_registered_identity.empty()
                            ? nullptr
                            : post_replay_last_registered_identity.c_str());
  std::printf(",\"post_replay_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(post_replay_last_replayed_identity.empty()
                            ? nullptr
                            : post_replay_last_replayed_identity.c_str());
  std::printf(",\"post_replay_imported_entry_status\":%d,",
              post_replay_imported_entry_status);
  std::printf("\"post_replay_local_entry_status\":%d,",
              post_replay_local_entry_status);
  std::printf("\"post_replay_imported_entry_found\":%d,",
              post_replay_imported_entry.found);
  std::printf("\"post_replay_local_entry_found\":%d,",
              post_replay_local_entry.found);
  std::printf("\"post_replay_imported_provider_class_value\":%d,",
              post_replay_imported_provider_class_value);
  std::printf("\"post_replay_imported_provider_protocol_value\":%d,",
              post_replay_imported_provider_protocol_value);
  std::printf("\"post_replay_local_consumer_class_value\":%d",
              post_replay_local_consumer_class_value);
  std::printf("}\n");
  return 0;
}
