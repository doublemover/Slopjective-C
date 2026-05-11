#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_PROBE_REPORT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_PROBE_REPORT_H_

#include "import_module_execution_matrix_probe/probe_result.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::import_module_execution_matrix {

inline void PrintStartupFixtureModuleFields(
    const StartupFixtureModuleState &state) {
  PrintIntField("startup_registration_copy_status",
                state.registration_copy_status);
  PrintUint64Field("startup_registered_image_count",
                   state.registration.registered_image_count);
  PrintUint64Field("startup_next_expected_registration_order_ordinal",
                   state.registration
                       .next_expected_registration_order_ordinal);
  PrintIntField("startup_image_walk_status", state.image_walk_status);
  PrintUint64Field("startup_walked_image_count",
                   state.image_walk.walked_image_count);
  PrintStringField("startup_last_walked_module_name",
                   state.image_walk.last_walked_module_name);
  PrintIntField("startup_graph_status", state.graph_status);
  PrintUint64Field("startup_realized_class_count",
                   state.graph.realized_class_count);
  PrintUint64Field("startup_root_class_count", state.graph.root_class_count);
  PrintUint64Field("startup_metaclass_edge_count",
                   state.graph.metaclass_edge_count);
  PrintIntField("imported_entry_status", state.imported_entry_status);
  PrintIntField("imported_entry_found", state.imported_entry.found);
  PrintUint64Field("imported_registration_order_ordinal",
                   state.imported_entry.registration_order_ordinal);
  PrintUint64Field("imported_direct_protocol_count",
                   state.imported_entry.direct_protocol_count);
  PrintUint64Field("imported_attached_protocol_count",
                   state.imported_entry.attached_protocol_count);
  PrintUint64Field("imported_runtime_property_accessor_count",
                   state.imported_entry.runtime_property_accessor_count);
  PrintStringField("imported_module_name",
                   state.imported_module_name.c_str());
  PrintStringField("imported_translation_unit_identity_key",
                   state.imported_translation_unit_identity_key.c_str());
  PrintStringField("imported_class_owner_identity",
                   state.imported_class_owner_identity.c_str());
  PrintIntField("local_entry_status", state.local_entry_status);
  PrintIntField("local_entry_found", state.local_entry.found);
  PrintUint64Field("local_registration_order_ordinal",
                   state.local_entry.registration_order_ordinal);
  PrintUint64Field("local_direct_protocol_count",
                   state.local_entry.direct_protocol_count);
  PrintUint64Field("local_attached_protocol_count",
                   state.local_entry.attached_protocol_count);
  PrintUint64Field("local_runtime_property_accessor_count",
                   state.local_entry.runtime_property_accessor_count);
  PrintStringField("local_module_name", state.local_module_name.c_str());
  PrintStringField("local_translation_unit_identity_key",
                   state.local_translation_unit_identity_key.c_str());
  PrintStringField("local_class_owner_identity",
                   state.local_class_owner_identity.c_str());
}

inline void PrintImportExecutionFields(
    const ImportExecutionCaseValues &values, const char *provider_class_field,
    const char *provider_protocol_field, const char *local_class_field) {
  PrintIntField(provider_class_field, values.imported_provider_class_value);
  PrintIntField(provider_protocol_field,
                values.imported_provider_protocol_value);
  PrintIntField(local_class_field, values.local_consumer_class_value);
}

inline void PrintStartupMatrixAssertionFields(
    const StartupMatrixAssertions &state) {
  PrintIntField("selector_table_status", state.selector_table_status);
  PrintUint64Field("selector_table_entry_count",
                   state.selector_table.selector_table_entry_count);
  PrintUint64Field("selector_metadata_backed_selector_count",
                   state.selector_table.metadata_backed_selector_count);
  PrintUint64Field("selector_dynamic_selector_count",
                   state.selector_table.dynamic_selector_count);
  PrintIntField("provider_selector_status", state.provider_selector_status);
  PrintIntField("provider_selector_found", state.provider_selector.found);
  PrintIntField("provider_selector_metadata_backed",
                state.provider_selector.metadata_backed);
  PrintUint64Field("provider_selector_provider_count",
                   state.provider_selector.metadata_provider_count);
  PrintUint64Field("provider_selector_first_ordinal",
                   state.provider_selector.first_registration_order_ordinal);
  PrintUint64Field("provider_selector_last_ordinal",
                   state.provider_selector.last_registration_order_ordinal);
  PrintIntField("imported_protocol_selector_status",
                state.imported_protocol_selector_status);
  PrintIntField("imported_protocol_selector_found",
                state.imported_protocol_selector.found);
  PrintIntField("imported_protocol_selector_metadata_backed",
                state.imported_protocol_selector.metadata_backed);
  PrintUint64Field("imported_protocol_selector_provider_count",
                   state.imported_protocol_selector.metadata_provider_count);
  PrintUint64Field(
      "imported_protocol_selector_first_ordinal",
      state.imported_protocol_selector.first_registration_order_ordinal);
  PrintUint64Field(
      "imported_protocol_selector_last_ordinal",
      state.imported_protocol_selector.last_registration_order_ordinal);
  PrintIntField("local_selector_status", state.local_selector_status);
  PrintIntField("local_selector_found", state.local_selector.found);
  PrintIntField("local_selector_metadata_backed",
                state.local_selector.metadata_backed);
  PrintUint64Field("local_selector_provider_count",
                   state.local_selector.metadata_provider_count);
  PrintUint64Field("local_selector_first_ordinal",
                   state.local_selector.first_registration_order_ordinal);
  PrintUint64Field("local_selector_last_ordinal",
                   state.local_selector.last_registration_order_ordinal);
  PrintIntField("method_cache_state_status", state.method_cache_state_status);
  PrintUint64Field("method_cache_entry_count",
                   state.method_cache_state.cache_entry_count);
  PrintUint64Field("method_cache_live_dispatch_count",
                   state.method_cache_state.live_dispatch_count);
  PrintUint64Field("method_cache_strict_dispatch_error_count",
                   state.method_cache_state.strict_dispatch_error_count);
  PrintStringField("method_cache_last_selector",
                   state.method_cache_last_selector.c_str());
  PrintStringField("method_cache_last_resolved_class_name",
                   state.method_cache_last_resolved_class_name.c_str());
  PrintStringField("method_cache_last_resolved_owner_identity",
                   state.method_cache_last_resolved_owner_identity.c_str());
  PrintIntField("provider_method_status", state.provider_method_status);
  PrintIntField("provider_method_found", state.provider_method.found);
  PrintIntField("provider_method_resolved", state.provider_method.resolved);
  PrintStringField("provider_method_owner_identity",
                   state.provider_method_owner_identity.c_str());
  PrintIntField("imported_protocol_method_status",
                state.imported_protocol_method_status);
  PrintIntField("imported_protocol_method_found",
                state.imported_protocol_method.found);
  PrintIntField("imported_protocol_method_resolved",
                state.imported_protocol_method.resolved);
  PrintStringField("imported_protocol_method_owner_identity",
                   state.imported_protocol_method_owner_identity.c_str());
  PrintIntField("local_method_status", state.local_method_status);
  PrintIntField("local_method_found", state.local_method.found);
  PrintIntField("local_method_resolved", state.local_method.resolved);
  PrintStringField("local_method_owner_identity",
                   state.local_method_owner_identity.c_str());
  PrintIntField("protocol_query_status", state.protocol_query_status);
  PrintIntField("protocol_query_class_found", state.protocol_query.class_found);
  PrintIntField("protocol_query_protocol_found",
                state.protocol_query.protocol_found);
  PrintIntField("protocol_query_conforms", state.protocol_query.conforms);
  PrintUint64Field("protocol_query_visited_protocol_count",
                   state.protocol_query.visited_protocol_count);
  PrintUint64Field("protocol_query_attached_category_count",
                   state.protocol_query.attached_category_count);
  PrintStringField("protocol_query_matched_protocol_owner_identity",
                   state.protocol_query.matched_protocol_owner_identity);
}

inline void PrintPostResetAndReplayFields(const PostResetRuntimeState &state,
                                          int replay_status) {
  PrintIntField("post_reset_registration_copy_status",
                state.registration_copy_status);
  PrintUint64Field("post_reset_registered_image_count",
                   state.registration.registered_image_count);
  PrintIntField("post_reset_replay_copy_status", state.replay_copy_status);
  PrintUint64Field("post_reset_retained_bootstrap_image_count",
                   state.replay.retained_bootstrap_image_count);
  PrintUint64Field("post_reset_generation", state.replay.reset_generation);
  PrintIntField("replay_status", replay_status);
}

inline void PrintPostReplayFixtureModuleFields(
    const PostReplayFixtureModuleState &state) {
  PrintIntField("post_replay_registration_copy_status",
                state.registration_copy_status);
  PrintUint64Field("post_replay_registered_image_count",
                   state.registration.registered_image_count);
  PrintUint64Field("post_replay_next_expected_registration_order_ordinal",
                   state.registration
                       .next_expected_registration_order_ordinal);
  PrintIntField("post_replay_image_walk_status", state.image_walk_status);
  PrintUint64Field("post_replay_walked_image_count",
                   state.image_walk.walked_image_count);
  PrintStringField("post_replay_last_walked_module_name",
                   state.image_walk.last_walked_module_name);
  PrintIntField("post_replay_graph_status", state.graph_status);
  PrintUint64Field("post_replay_realized_class_count",
                   state.graph.realized_class_count);
  PrintIntField("post_replay_replay_copy_status", state.replay_copy_status);
  PrintUint64Field("post_replay_replay_generation",
                   state.replay.replay_generation);
  PrintUint64Field("post_replay_retained_bootstrap_image_count",
                   state.replay.retained_bootstrap_image_count);
  PrintIntField("post_replay_imported_entry_status",
                state.imported_entry_status);
  PrintIntField("post_replay_imported_entry_found",
                state.imported_entry.found);
  PrintStringField("post_replay_imported_module_name",
                   state.imported_entry.module_name);
  PrintStringField("post_replay_imported_translation_unit_identity_key",
                   state.imported_entry.translation_unit_identity_key);
  PrintIntField("post_replay_local_entry_status", state.local_entry_status);
  PrintIntField("post_replay_local_entry_found", state.local_entry.found);
  PrintStringField("post_replay_local_module_name",
                   state.local_entry.module_name);
  PrintStringField("post_replay_local_translation_unit_identity_key",
                   state.local_entry.translation_unit_identity_key);
}

inline void PrintPostReplayMatrixAssertionFields(
    const PostReplayMatrixAssertions &state) {
  PrintIntField("post_replay_selector_table_status",
                state.selector_table_status);
  PrintUint64Field("post_replay_selector_table_entry_count",
                   state.selector_table.selector_table_entry_count);
  PrintUint64Field("post_replay_selector_metadata_backed_selector_count",
                   state.selector_table.metadata_backed_selector_count);
  PrintIntField("post_replay_provider_selector_status",
                state.provider_selector_status);
  PrintIntField("post_replay_provider_selector_found",
                state.provider_selector.found);
  PrintIntField("post_replay_imported_protocol_selector_status",
                state.imported_protocol_selector_status);
  PrintIntField("post_replay_imported_protocol_selector_found",
                state.imported_protocol_selector.found);
  PrintIntField("post_replay_local_selector_status",
                state.local_selector_status);
  PrintIntField("post_replay_local_selector_found",
                state.local_selector.found);
  PrintIntField("post_replay_method_cache_state_status",
                state.method_cache_state_status);
  PrintUint64Field("post_replay_method_cache_entry_count",
                   state.method_cache_state.cache_entry_count);
  PrintUint64Field("post_replay_method_cache_live_dispatch_count",
                   state.method_cache_state.live_dispatch_count);
  PrintUint64Field("post_replay_method_cache_strict_dispatch_error_count",
                   state.method_cache_state.strict_dispatch_error_count);
  PrintStringField("post_replay_method_cache_last_selector",
                   state.method_cache_state.last_selector);
  PrintStringField("post_replay_method_cache_last_resolved_class_name",
                   state.method_cache_state.last_resolved_class_name);
  PrintStringField("post_replay_method_cache_last_resolved_owner_identity",
                   state.method_cache_state.last_resolved_owner_identity);
  PrintIntField("post_replay_provider_method_status",
                state.provider_method_status);
  PrintIntField("post_replay_provider_method_found",
                state.provider_method.found);
  PrintIntField("post_replay_provider_method_resolved",
                state.provider_method.resolved);
  PrintStringField("post_replay_provider_method_owner_identity",
                   state.provider_method.resolved_owner_identity);
  PrintIntField("post_replay_imported_protocol_method_status",
                state.imported_protocol_method_status);
  PrintIntField("post_replay_imported_protocol_method_found",
                state.imported_protocol_method.found);
  PrintIntField("post_replay_imported_protocol_method_resolved",
                state.imported_protocol_method.resolved);
  PrintStringField("post_replay_imported_protocol_method_owner_identity",
                   state.imported_protocol_method.resolved_owner_identity);
  PrintIntField("post_replay_local_method_status", state.local_method_status);
  PrintIntField("post_replay_local_method_found", state.local_method.found);
  PrintIntField("post_replay_local_method_resolved",
                state.local_method.resolved);
  PrintStringField("post_replay_local_method_owner_identity",
                   state.local_method.resolved_owner_identity, false);
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintStartupFixtureModuleFields(result.startup_fixture);
  PrintImportExecutionFields(
      result.startup_execution, "imported_provider_class_value",
      "imported_provider_protocol_value", "local_consumer_class_value");
  PrintStartupMatrixAssertionFields(result.startup_matrix);
  PrintPostResetAndReplayFields(result.post_reset, result.replay_status);
  PrintPostReplayFixtureModuleFields(result.post_replay_fixture);
  PrintImportExecutionFields(
      result.post_replay_execution,
      "post_replay_imported_provider_class_value",
      "post_replay_imported_provider_protocol_value",
      "post_replay_local_consumer_class_value");
  PrintPostReplayMatrixAssertionFields(result.post_replay_matrix);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::import_module_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_IMPORT_MODULE_EXECUTION_MATRIX_PROBE_PROBE_REPORT_H_
