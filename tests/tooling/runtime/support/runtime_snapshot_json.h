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

inline void PrintMethodCacheEntryWithProbeCounts(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
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
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
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
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
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
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
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
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf(
      "\"last_category_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf(
      "\"last_protocol_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
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
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf(
      "\"last_selector_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf(
      "\"last_category_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf(
      "\"last_protocol_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
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
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
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
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
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
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(
      ",\"last_selector_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
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
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(
      ",\"last_selector_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf(
      "\"last_category_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf(
      "\"last_protocol_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintRealizedGraphStateAllocation(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("realized_class_count", static_cast<unsigned long long>(
                                               snapshot.realized_class_count));
  PrintUint64Field("root_class_count",
                   static_cast<unsigned long long>(snapshot.root_class_count));
  PrintUint64Field("metaclass_edge_count", static_cast<unsigned long long>(
                                               snapshot.metaclass_edge_count));
  PrintUint64Field(
      "receiver_class_binding_count",
      static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field("protocol_conformance_edge_count",
                   static_cast<unsigned long long>(
                       snapshot.protocol_conformance_edge_count));
  PrintUint64Field("live_instance_count", static_cast<unsigned long long>(
                                              snapshot.live_instance_count));
  PrintUint64Field("last_allocated_receiver_identity",
                   static_cast<unsigned long long>(
                       snapshot.last_allocated_receiver_identity));
  PrintUint64Field(
      "last_allocated_base_identity",
      static_cast<unsigned long long>(snapshot.last_allocated_base_identity));
  PrintUint64Field("last_allocated_instance_size_bytes",
                   static_cast<unsigned long long>(
                       snapshot.last_allocated_instance_size_bytes));
  PrintStringField("last_realized_class_name",
                   snapshot.last_realized_class_name);
  PrintStringField("last_realized_class_owner_identity",
                   snapshot.last_realized_class_owner_identity);
  PrintStringField("last_realized_metaclass_owner_identity",
                   snapshot.last_realized_metaclass_owner_identity);
  PrintStringField("last_allocated_class_name",
                   snapshot.last_allocated_class_name, false);
  std::printf("}");
}

inline void PrintRealizedClassEntryAllocation(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  PrintIntField("is_root_class", snapshot.is_root_class);
  PrintIntField("implementation_backed", snapshot.implementation_backed);
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field(
      "direct_protocol_count",
      static_cast<unsigned long long>(snapshot.direct_protocol_count));
  PrintUint64Field(
      "attached_protocol_count",
      static_cast<unsigned long long>(snapshot.attached_protocol_count));
  PrintUint64Field("runtime_property_accessor_count",
                   static_cast<unsigned long long>(
                       snapshot.runtime_property_accessor_count));
  PrintUint64Field(
      "runtime_instance_size_bytes",
      static_cast<unsigned long long>(snapshot.runtime_instance_size_bytes));
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("metaclass_owner_identity",
                   snapshot.metaclass_owner_identity, false);
  std::printf("}");
}

inline void PrintRealizedGraphStateMetaclass(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("realized_class_count", static_cast<unsigned long long>(
                                               snapshot.realized_class_count));
  PrintUint64Field("root_class_count",
                   static_cast<unsigned long long>(snapshot.root_class_count));
  PrintUint64Field("metaclass_edge_count", static_cast<unsigned long long>(
                                               snapshot.metaclass_edge_count));
  PrintUint64Field(
      "receiver_class_binding_count",
      static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  PrintStringField("last_realized_class_name",
                   snapshot.last_realized_class_name);
  PrintStringField("last_realized_class_owner_identity",
                   snapshot.last_realized_class_owner_identity);
  PrintStringField("last_realized_metaclass_owner_identity",
                   snapshot.last_realized_metaclass_owner_identity, false);
  std::printf("}");
}

inline void PrintRealizedEntryMetaclass(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  PrintIntField("is_root_class", snapshot.is_root_class);
  PrintIntField("implementation_backed", snapshot.implementation_backed);
  PrintStringField("module_name", snapshot.module_name);
  PrintStringField("translation_unit_identity_key",
                   snapshot.translation_unit_identity_key);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("metaclass_owner_identity",
                   snapshot.metaclass_owner_identity);
  PrintStringField("super_class_owner_identity",
                   snapshot.super_class_owner_identity);
  PrintStringField("super_metaclass_owner_identity",
                   snapshot.super_metaclass_owner_identity, false);
  std::printf("}");
}

inline void PrintGraphStateProtocolCategory(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("realized_class_count", static_cast<unsigned long long>(
                                               snapshot.realized_class_count));
  PrintUint64Field("root_class_count",
                   static_cast<unsigned long long>(snapshot.root_class_count));
  PrintUint64Field("metaclass_edge_count", static_cast<unsigned long long>(
                                               snapshot.metaclass_edge_count));
  PrintUint64Field(
      "receiver_class_binding_count",
      static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field("protocol_conformance_edge_count",
                   static_cast<unsigned long long>(
                       snapshot.protocol_conformance_edge_count));
  PrintStringField("last_realized_class_name",
                   snapshot.last_realized_class_name);
  PrintStringField("last_realized_class_owner_identity",
                   snapshot.last_realized_class_owner_identity);
  PrintStringField("last_realized_metaclass_owner_identity",
                   snapshot.last_realized_metaclass_owner_identity);
  PrintStringField("last_attached_category_owner_identity",
                   snapshot.last_attached_category_owner_identity);
  PrintStringField("last_attached_category_name",
                   snapshot.last_attached_category_name, false);
  std::printf("}");
}

inline void PrintRealizedEntryProtocolCategory(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  PrintIntField("is_root_class", snapshot.is_root_class);
  PrintIntField("implementation_backed", snapshot.implementation_backed);
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field(
      "direct_protocol_count",
      static_cast<unsigned long long>(snapshot.direct_protocol_count));
  PrintUint64Field(
      "attached_protocol_count",
      static_cast<unsigned long long>(snapshot.attached_protocol_count));
  PrintStringField("module_name", snapshot.module_name);
  PrintStringField("translation_unit_identity_key",
                   snapshot.translation_unit_identity_key);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("metaclass_owner_identity",
                   snapshot.metaclass_owner_identity);
  PrintStringField("super_class_owner_identity",
                   snapshot.super_class_owner_identity);
  PrintStringField("super_metaclass_owner_identity",
                   snapshot.super_metaclass_owner_identity);
  PrintStringField("last_attached_category_owner_identity",
                   snapshot.last_attached_category_owner_identity);
  PrintStringField("last_attached_category_name",
                   snapshot.last_attached_category_name, false);
  std::printf("}");
}

inline void PrintConformanceQueryProtocolCategory(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("class_found", snapshot.class_found);
  PrintIntField("protocol_found", snapshot.protocol_found);
  PrintIntField("conforms", snapshot.conforms);
  PrintUint64Field(
      "visited_protocol_count",
      static_cast<unsigned long long>(snapshot.visited_protocol_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("protocol_name", snapshot.protocol_name);
  PrintStringField("matched_protocol_owner_identity",
                   snapshot.matched_protocol_owner_identity);
  PrintStringField("matched_attachment_owner_identity",
                   snapshot.matched_attachment_owner_identity, false);
  std::printf("}");
}

inline void PrintRealizedClassEntryCanonicalSummary(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("runtime_property_accessor_count",
                   static_cast<unsigned long long>(
                       snapshot.runtime_property_accessor_count));
  PrintUint64Field(
      "runtime_instance_size_bytes",
      static_cast<unsigned long long>(snapshot.runtime_instance_size_bytes));
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("last_attached_category_owner_identity",
                   snapshot.last_attached_category_owner_identity, false);
  std::printf("}");
}

inline void PrintConformanceQueryCanonicalSummary(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("conforms", snapshot.conforms);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("protocol_name", snapshot.protocol_name);
  PrintStringField("matched_protocol_owner_identity",
                   snapshot.matched_protocol_owner_identity);
  PrintStringField("matched_attachment_owner_identity",
                   snapshot.matched_attachment_owner_identity, false);
  std::printf("}");
}

inline void PrintPropertyEntryCanonicalSummary(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintUint64Field("size_bytes",
                   static_cast<unsigned long long>(snapshot.size_bytes));
  PrintUint64Field("alignment_bytes",
                   static_cast<unsigned long long>(snapshot.alignment_bytes));
  PrintUint64Field("instance_size_bytes", static_cast<unsigned long long>(
                                              snapshot.instance_size_bytes));
  PrintStringField("property_name", snapshot.property_name);
  PrintStringField("effective_getter_selector",
                   snapshot.effective_getter_selector);
  PrintStringField("effective_setter_selector",
                   snapshot.effective_setter_selector);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintPropertyRegistryStateFull(
    const objc3_runtime_property_registry_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field(
      "layout_ready_class_count",
      static_cast<unsigned long long>(snapshot.layout_ready_class_count));
  PrintUint64Field(
      "reflectable_property_count",
      static_cast<unsigned long long>(snapshot.reflectable_property_count));
  PrintUint64Field(
      "writable_property_count",
      static_cast<unsigned long long>(snapshot.writable_property_count));
  PrintUint64Field(
      "slot_backed_property_count",
      static_cast<unsigned long long>(snapshot.slot_backed_property_count));
  PrintIntField("last_query_found", snapshot.last_query_found);
  PrintIntField("last_query_inherited", snapshot.last_query_inherited);
  PrintStringField("last_queried_class_name", snapshot.last_queried_class_name);
  PrintStringField("last_queried_property_name",
                   snapshot.last_queried_property_name);
  PrintStringField("last_resolved_class_name",
                   snapshot.last_resolved_class_name);
  PrintStringField("last_resolved_owner_identity",
                   snapshot.last_resolved_owner_identity, false);
  std::printf("}");
}

inline void
PrintPropertyEntryFull(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("inherited", snapshot.inherited);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintUint64Field("size_bytes",
                   static_cast<unsigned long long>(snapshot.size_bytes));
  PrintUint64Field("alignment_bytes",
                   static_cast<unsigned long long>(snapshot.alignment_bytes));
  PrintUint64Field("instance_size_bytes", static_cast<unsigned long long>(
                                              snapshot.instance_size_bytes));
  PrintStringField("queried_class_name", snapshot.queried_class_name);
  PrintStringField("resolved_class_name", snapshot.resolved_class_name);
  PrintStringField("property_name", snapshot.property_name);
  PrintStringField("declaration_owner_identity",
                   snapshot.declaration_owner_identity);
  PrintStringField("export_owner_identity", snapshot.export_owner_identity);
  PrintStringField("getter_selector", snapshot.getter_selector);
  PrintStringField("setter_selector", snapshot.setter_selector);
  PrintStringField("effective_getter_selector",
                   snapshot.effective_getter_selector);
  PrintStringField("effective_setter_selector",
                   snapshot.effective_setter_selector);
  PrintStringField("ivar_binding_symbol", snapshot.ivar_binding_symbol);
  PrintStringField("synthesized_binding_symbol",
                   snapshot.synthesized_binding_symbol);
  PrintStringField("ivar_layout_symbol", snapshot.ivar_layout_symbol);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintRealizedClassEntryPropertySummary(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("runtime_property_accessor_count",
                   static_cast<unsigned long long>(
                       snapshot.runtime_property_accessor_count));
  PrintUint64Field(
      "runtime_instance_size_bytes",
      static_cast<unsigned long long>(snapshot.runtime_instance_size_bytes));
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintDispatchStatePropertyExecution(
    const objc3_runtime_dispatch_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("cache_entry_count",
                   static_cast<unsigned long long>(snapshot.cache_entry_count));
  PrintUint64Field("fast_path_seed_count", static_cast<unsigned long long>(
                                               snapshot.fast_path_seed_count));
  PrintUint64Field("fast_path_hit_count", static_cast<unsigned long long>(
                                              snapshot.fast_path_hit_count));
  PrintUint64Field("live_dispatch_count", static_cast<unsigned long long>(
                                              snapshot.live_dispatch_count));
  PrintUint64Field(
      "strict_dispatch_error_count",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  PrintUint64Field(
      "last_resolved_parameter_count",
      static_cast<unsigned long long>(snapshot.last_resolved_parameter_count));
  PrintUint64Field(
      "last_property_base_identity",
      static_cast<unsigned long long>(snapshot.last_property_base_identity));
  PrintUint64Field(
      "last_property_slot_index",
      static_cast<unsigned long long>(snapshot.last_property_slot_index));
  PrintIntField("last_dispatch_used_cache", snapshot.last_dispatch_used_cache);
  PrintIntField("last_dispatch_used_fast_path",
                snapshot.last_dispatch_used_fast_path);
  PrintIntField("last_dispatch_resolved_live_method",
                snapshot.last_dispatch_resolved_live_method);
  PrintIntField("last_dispatch_strict_error",
                snapshot.last_dispatch_strict_error);
  PrintIntField("last_effective_direct_dispatch",
                snapshot.last_effective_direct_dispatch);
  PrintIntField("last_used_builtin", snapshot.last_used_builtin);
  PrintStringField("last_selector", snapshot.last_selector);
  PrintStringField("last_fast_path_reason", snapshot.last_fast_path_reason);
  PrintStringField("last_dispatch_path", snapshot.last_dispatch_path);
  PrintStringField("last_implementation_kind",
                   snapshot.last_implementation_kind);
  PrintStringField("last_property_name", snapshot.last_property_name);
  PrintStringField("last_resolved_class_name",
                   snapshot.last_resolved_class_name);
  PrintStringField("last_resolved_owner_identity",
                   snapshot.last_resolved_owner_identity, false);
  std::printf("}");
}

inline void PrintAllocationGraph(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("live_instance_count", static_cast<unsigned long long>(
                                              snapshot.live_instance_count));
  PrintStringField("last_allocated_class_name",
                   snapshot.last_allocated_class_name, false);
  std::printf("}");
}

inline void PrintPropertyEntryOwnershipHook(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("inherited", snapshot.inherited);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintMemoryManagementState(
    const objc3_runtime_memory_management_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field(
      "live_runtime_instance_count",
      static_cast<unsigned long long>(snapshot.live_runtime_instance_count));
  PrintUint64Field("weak_target_count",
                   static_cast<unsigned long long>(snapshot.weak_target_count));
  PrintUint64Field("weak_slot_ref_count", static_cast<unsigned long long>(
                                              snapshot.weak_slot_ref_count));
  PrintUint64Field(
      "autoreleasepool_depth",
      static_cast<unsigned long long>(snapshot.autoreleasepool_depth));
  PrintUint64Field(
      "autoreleasepool_max_depth",
      static_cast<unsigned long long>(snapshot.autoreleasepool_max_depth));
  PrintUint64Field(
      "queued_autorelease_value_count",
      static_cast<unsigned long long>(snapshot.queued_autorelease_value_count));
  PrintUint64Field("drained_autorelease_value_count",
                   static_cast<unsigned long long>(
                       snapshot.drained_autorelease_value_count));
  PrintIntField("last_autoreleased_value", snapshot.last_autoreleased_value);
  PrintIntField("last_drained_autorelease_value",
                snapshot.last_drained_autorelease_value, false);
  std::printf("}");
}

inline void PrintPropertyEntryWeakAutoreleasepool(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile, false);
  std::printf("}");
}

inline void PrintPropertyEntryStorageOwnership(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintUint64Field("size_bytes",
                   static_cast<unsigned long long>(snapshot.size_bytes));
  PrintUint64Field("alignment_bytes",
                   static_cast<unsigned long long>(snapshot.alignment_bytes));
  PrintUint64Field("instance_size_bytes", static_cast<unsigned long long>(
                                              snapshot.instance_size_bytes));
  PrintStringField("queried_class_name", snapshot.queried_class_name);
  PrintStringField("resolved_class_name", snapshot.resolved_class_name);
  PrintStringField("property_name", snapshot.property_name);
  PrintStringField("effective_getter_selector",
                   snapshot.effective_getter_selector);
  PrintStringField("effective_setter_selector",
                   snapshot.effective_setter_selector);
  PrintStringField("property_attribute_profile",
                   snapshot.property_attribute_profile);
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}

} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_JSON_H_
