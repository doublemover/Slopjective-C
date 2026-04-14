#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_JSON_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_JSON_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe {

inline void PrintRegistrationStateFull(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
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
  std::printf("\"registered_descriptor_total\":%llu,",
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
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf(
      "\"next_expected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          snapshot.next_expected_registration_order_ordinal));
  std::printf(
      "\"last_successful_registration_order_ordinal\":%llu,",
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
  std::printf("\"selector_table_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf("\"metadata_backed_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_backed_selector_count));
  std::printf("\"dynamic_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.dynamic_selector_count));
  std::printf("\"metadata_provider_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_provider_edge_count));
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
  std::printf(",\"last_materialized_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_materialized_stable_id));
  std::printf(
      "\"last_materialized_registration_order_ordinal\":%llu,",
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
  std::printf("\"selector_table_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf("\"metadata_backed_selector_count\":%llu,",
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
  std::printf("\"selector_table_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf("\"metadata_backed_selector_count\":%llu,",
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
  std::printf("\"metadata_provider_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_provider_count));
  std::printf("\"first_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.first_registration_order_ordinal));
  std::printf("\"last_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_registration_order_ordinal));
  std::printf("\"first_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(snapshot.first_selector_pool_index));
  std::printf("\"last_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(snapshot.last_selector_pool_index));
  std::printf("\"canonical_selector\":");
  PrintJsonStringOrNull(snapshot.canonical_selector);
  std::printf("}");
}

inline void PrintMethodCacheEntryWithProbeCounts(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf("\"normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"parameter_count\":%llu,",
              static_cast<unsigned long long>(snapshot.parameter_count));
  std::printf("\"category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.category_probe_count));
  std::printf("\"protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_probe_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheEntryBasic(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf("\"normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"parameter_count\":%llu,",
              static_cast<unsigned long long>(snapshot.parameter_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheEntryMetaclassMinimal(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf("\"normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.normalized_receiver_identity));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheEntryRuntimeCanonical(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf("\"normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.category_probe_count));
  std::printf("\"protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_probe_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateCategoryAttachment(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf("\"last_protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateFull(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf("\"last_protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateMetaclass(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateMethodBinding(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateSlowPath(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateProtocolCategory(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf("\"last_protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_JSON_H_
