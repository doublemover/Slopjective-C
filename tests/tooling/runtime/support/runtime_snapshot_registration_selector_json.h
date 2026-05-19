#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REGISTRATION_SELECTOR_JSON_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REGISTRATION_SELECTOR_JSON_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe {

inline void PrintRegistrationStateFull(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf(
      "\"registered_descriptor_total\":%llu,",
      static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.next_expected_registration_order_ordinal));
  std::printf("\"last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_successful_registration_order_ordinal));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
  std::printf(",\"last_rejected_module_name\":");
  PrintJsonStringOrNull(snapshot.last_rejected_module_name);
  std::printf(",\"last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_rejected_translation_unit_identity_key);
  std::printf(",\"last_rejected_registration_order_ordinal\":%llu",
              static_cast<unsigned long long>(
                  snapshot.last_rejected_registration_order_ordinal));
  std::printf("}");
}

inline void PrintRegistrationStateBasic(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf(
      "\"registered_descriptor_total\":%llu,",
      static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
  std::printf("}");
}

inline void PrintRegistrationStateCountsOnly(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf(
      "\"registered_descriptor_total\":%llu,",
      static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"last_registration_status\":%d",
              snapshot.last_registration_status);
  std::printf("}");
}

inline void PrintRegistrationStateSelectorLookup(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf(
      "\"registered_descriptor_total\":%llu,",
      static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.next_expected_registration_order_ordinal));
  std::printf("\"last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_successful_registration_order_ordinal));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
  std::printf(",\"last_rejected_registration_order_ordinal\":%llu",
              static_cast<unsigned long long>(
                  snapshot.last_rejected_registration_order_ordinal));
  std::printf("}");
}

inline void PrintSelectorTableStateFull(
    const objc3_runtime_selector_lookup_table_state_snapshot &snapshot) {
  std::printf("{");
  std::printf(
      "\"selector_table_entry_count\":%llu,",
      static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf(
      "\"metadata_backed_selector_count\":%llu,",
      static_cast<unsigned long long>(snapshot.metadata_backed_selector_count));
  std::printf("\"dynamic_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.dynamic_selector_count));
  std::printf(
      "\"metadata_provider_edge_count\":%llu,",
      static_cast<unsigned long long>(snapshot.metadata_provider_edge_count));
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
  std::printf(
      ",\"last_materialized_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_materialized_stable_id));
  std::printf("\"last_materialized_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_materialized_registration_order_ordinal));
  std::printf("\"last_materialized_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_materialized_selector_pool_index));
  std::printf("\"last_materialized_from_metadata\":%d",
              snapshot.last_materialized_from_metadata);
  std::printf("}");
}

inline void PrintSelectorTableStateBasic(
    const objc3_runtime_selector_lookup_table_state_snapshot &snapshot) {
  std::printf("{");
  std::printf(
      "\"selector_table_entry_count\":%llu,",
      static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf(
      "\"metadata_backed_selector_count\":%llu,",
      static_cast<unsigned long long>(snapshot.metadata_backed_selector_count));
  std::printf("\"dynamic_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.dynamic_selector_count));
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
  std::printf("}");
}

inline void PrintSelectorTableStateRuntimeCanonical(
    const objc3_runtime_selector_lookup_table_state_snapshot &snapshot) {
  std::printf("{");
  std::printf(
      "\"selector_table_entry_count\":%llu,",
      static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf(
      "\"metadata_backed_selector_count\":%llu,",
      static_cast<unsigned long long>(snapshot.metadata_backed_selector_count));
  std::printf("\"dynamic_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.dynamic_selector_count));
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
  std::printf(",\"last_materialized_from_metadata\":%d",
              snapshot.last_materialized_from_metadata);
  std::printf("}");
}

inline void PrintSelectorEntryBasic(
    const objc3_runtime_selector_lookup_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"metadata_backed\":%d,", snapshot.metadata_backed);
  std::printf("\"stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.stable_id));
  std::printf("\"canonical_selector\":");
  PrintJsonStringOrNull(snapshot.canonical_selector);
  std::printf("}");
}

inline void PrintSelectorEntryFull(
    const objc3_runtime_selector_lookup_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"metadata_backed\":%d,", snapshot.metadata_backed);
  std::printf("\"stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.stable_id));
  std::printf(
      "\"metadata_provider_count\":%llu,",
      static_cast<unsigned long long>(snapshot.metadata_provider_count));
  std::printf("\"first_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.first_registration_order_ordinal));
  std::printf("\"last_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_registration_order_ordinal));
  std::printf(
      "\"first_selector_pool_index\":%llu,",
      static_cast<unsigned long long>(snapshot.first_selector_pool_index));
  std::printf(
      "\"last_selector_pool_index\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_pool_index));
  std::printf("\"canonical_selector\":");
  PrintJsonStringOrNull(snapshot.canonical_selector);
  std::printf("}");
}


} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REGISTRATION_SELECTOR_JSON_H_
