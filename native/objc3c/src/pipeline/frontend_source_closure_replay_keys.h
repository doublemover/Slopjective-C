#pragma once

#include <sstream>
#include <string>

#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/frontend_type_source_closure.h"
#include "sema/model/semantic_ownership.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

inline std::string BuildTypeSystemTypeSourceClosureReplayKey(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";protocol_required_methods=" << summary.protocol_required_method_count
      << ";protocol_optional_methods=" << summary.protocol_optional_method_count
      << ";protocol_required_properties=" << summary.protocol_required_property_count
      << ";protocol_optional_properties=" << summary.protocol_optional_property_count
      << ";object_pointer_type_spelling_sites=" << summary.object_pointer_type_spelling_sites
      << ";pointer_declarator_entries=" << summary.pointer_declarator_entries
      << ";nullability_suffix_entries=" << summary.nullability_suffix_entries
      << ";generic_suffix_entries=" << summary.generic_suffix_entries
      << ";optional_sites="
      << summary.optional_binding_sites << ":" << summary.guard_binding_sites << ":"
      << summary.optional_send_sites << ":" << summary.nil_coalescing_sites << ":"
      << summary.typed_keypath_literal_sites
      << ";optional_member_access_sites=" << summary.optional_member_access_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildControlFlowControlFlowSourceClosureReplayKey(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";guard_binding_sites=" << summary.guard_binding_sites
      << ";guard_binding_clause_sites=" << summary.guard_binding_clause_sites
      << ";guard_boolean_condition_sites=" << summary.guard_boolean_condition_sites
      << ";switch_pattern_sites=" << summary.switch_case_pattern_sites << ":"
      << summary.switch_default_pattern_sites
      << ";match_surface_sites=" << summary.match_statement_sites << ":"
      << summary.match_case_pattern_sites << ":" << summary.match_default_sites << ":"
      << summary.match_wildcard_pattern_sites << ":"
      << summary.match_literal_pattern_sites << ":"
      << summary.match_binding_pattern_sites << ":"
      << summary.match_result_case_pattern_sites
      << ";reserved_keyword_sites=" << summary.defer_keyword_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildErrorHandlingErrorSourceClosureReplayKey(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";throws_sites=" << summary.function_throws_declaration_sites << ":"
      << summary.method_throws_declaration_sites
      << ";result_sites=" << summary.result_like_sites << ":"
      << summary.result_success_sites << ":" << summary.result_failure_sites << ":"
      << summary.result_branch_sites << ":" << summary.result_payload_sites
      << ";nserror_sites=" << summary.ns_error_bridging_sites << ":"
      << summary.ns_error_out_parameter_sites << ":"
      << summary.ns_error_bridge_path_sites
      << ";bridge_marker_sites=" << summary.objc_nserror_attribute_sites << ":"
      << summary.objc_status_code_attribute_sites << ":"
      << summary.status_code_success_clause_sites << ":"
      << summary.status_code_error_type_clause_sites << ":"
      << summary.status_code_mapping_clause_sites
      << ";reserved_keyword_sites=" << summary.try_keyword_sites << ":"
      << summary.throw_keyword_sites << ":" << summary.catch_keyword_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildConcurrencyAsyncSourceClosureReplayKey(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_sites=" << summary.async_keyword_sites << ":"
      << summary.async_function_sites << ":" << summary.async_method_sites
      << ";await_sites=" << summary.await_keyword_sites << ":"
      << summary.await_expression_sites
      << ";executor_sites=" << summary.executor_attribute_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites << ":"
      << summary.executor_named_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";actor_sites=" << summary.actor_interface_sites << ":"
      << summary.actor_method_sites << ":" << summary.actor_property_sites
      << ";nonisolated_sites=" << summary.objc_nonisolated_annotation_sites
      << ";executor_sites=" << summary.actor_member_executor_annotation_sites
      << ";async_sites=" << summary.actor_async_method_sites
      << ";metadata_sites=" << summary.actor_member_metadata_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildConcurrencyTaskGroupCancellationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_callable_sites=" << summary.async_callable_sites
      << ";executor_sites=" << summary.executor_attribute_sites
      << ";task_creation_sites=" << summary.task_creation_sites
      << ";task_group_sites=" << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";cancellation_sites=" << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildOwnershipSystemExtensionSourceClosureReplayKey(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";borrowed_sites=" << summary.borrowed_pointer_sites
      << ";returns_borrowed_sites=" << summary.returns_borrowed_attribute_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildOwnershipCleanupResourceCaptureSourceCompletionReplayKey(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";cleanup_sites=" << summary.cleanup_attribute_sites << ":"
      << summary.cleanup_sugar_sites
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_sugar_sites << ":" << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildOwnershipRetainableCFamilySourceCompletionReplayKey(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";family_sites=" << summary.family_retain_sites << ":"
      << summary.family_release_sites << ":" << summary.family_autorelease_sites
      << ";compat_sites=" << summary.compatibility_returns_retained_sites << ":"
      << summary.compatibility_returns_not_retained_sites << ":"
      << summary.compatibility_consumed_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildDispatchDispatchIntentSourceClosureReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";callable_sites=" << summary.direct_callable_sites << ":"
      << summary.final_callable_sites << ":" << summary.dynamic_callable_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites << ":"
      << summary.actor_container_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildDispatchDispatchIntentSourceCompletionReplayKey(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";prefixed_container_sites=" << summary.prefixed_container_attribute_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ";defaulting_sites=" << summary.effective_direct_member_sites << ":"
      << summary.direct_members_defaulted_method_sites << ":"
      << summary.direct_members_dynamic_opt_out_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.derive_marker_sites << ":"
      << summary.macro_marker_sites << ":" << summary.property_behavior_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.macro_marker_sites << ":" << summary.macro_package_sites
      << ":" << summary.macro_provenance_sites << ":"
      << summary.macro_cache_key_sites << ":" << summary.macro_sandbox_policy_sites
      << ":" << summary.expansion_visible_macro_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.property_behavior_sites << ":"
      << summary.interface_property_behavior_sites << ":"
      << summary.implementation_property_behavior_sites << ":"
      << summary.protocol_property_behavior_sites << ":"
      << summary.synthesized_binding_visible_sites << ":"
      << summary.synthesized_getter_visible_sites << ":"
      << summary.synthesized_setter_visible_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildInteropForeignImportSourceClosureReplayKey(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.foreign_callable_sites << ":"
      << summary.extern_foreign_callable_sites << ":"
      << summary.import_module_annotation_sites << ":"
      << summary.imported_module_name_sites << ":"
      << summary.export_header_annotation_sites << ":"
      << summary.export_header_name_sites << ":" << summary.mixed_image_annotation_sites
      << ":" << summary.mixed_image_name_sites << ":"
      << summary.package_entry_annotation_sites << ":"
      << summary.package_entry_name_sites << ":" << summary.interop_annotation_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildInteropCppSwiftInteropAnnotationSourceCompletionReplayKey(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.swift_name_annotation_sites << ":"
      << summary.swift_private_annotation_sites << ":" << summary.cpp_name_annotation_sites
      << ":" << summary.header_name_annotation_sites << ":"
      << summary.abi_alignment_annotation_sites << ":"
      << summary.foreign_type_annotation_sites << ":"
      << summary.interop_metadata_annotation_sites << ":"
      << summary.named_annotation_payload_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildToolingDiagnosticsMigratorSourceInventoryReplayKey(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary &summary) {
  std::ostringstream out;
  out << "families=" << summary.advanced_feature_family_count
      << ";dependencies=" << summary.dependency_surface_count
      << ";claims=" << summary.aggregated_source_only_claim_count
      << ";fail-closed=" << summary.fail_closed_construct_count
      << ";sites=" << summary.diagnostic_surface_sites << ":"
      << summary.fixit_surface_sites << ":" << summary.migrator_surface_sites << ":"
      << summary.canonicalization_hint_sites
      << ";parts=" << summary.error_surface_sites << ":"
      << summary.concurrency_surface_sites << ":" << summary.system_surface_sites << ":"
      << summary.dispatch_surface_sites << ":" << summary.metaprogramming_surface_sites
      << ":" << summary.interop_surface_sites
      << ";supported="
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ":" << (summary.fixit_inventory_source_supported ? "true" : "false")
      << ":" << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildToolingMigrationCanonicalizationSourceCompletionReplayKey(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << "compat=" << summary.language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ";canonical-literal-rejections=" << summary.legacy_yes_sites << ":"
      << summary.legacy_no_sites << ":" << summary.legacy_null_sites << ":"
      << summary.legacy_total_sites
      << ";canonical=" << summary.canonical_true_rewrite_sites << ":"
      << summary.canonical_false_rewrite_sites << ":"
      << summary.canonical_nil_rewrite_sites
      << ";candidates=" << summary.canonicalization_candidate_sites << ":"
      << summary.fixit_candidate_sites << ":" << summary.migrator_candidate_sites
      << ";ready=" << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildToolingDiagnosticTaxonomyPortabilityContractReplayKey(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "portability-dependencies=" << summary.portability_dependency_count
      << ";diagnostics=" << summary.diagnostics_total << ":"
      << summary.diagnostics_after_pass_final << ":" << summary.diagnostics_emitted_total
      << ";arc-fixit=" << summary.ownership_arc_diagnostic_candidate_sites << ":"
      << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_profiled_sites << ":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites << ":"
      << summary.ownership_arc_contract_violation_sites
      << ";migration-candidates=" << summary.migration_canonicalization_candidate_sites
      << ";ready=" << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildToolingFeatureSpecificFixitSynthesisReplayKey(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "families=" << summary.fixit_family_count
      << ";migration=" << summary.migration_fixit_candidate_sites << ":"
      << summary.migrator_candidate_sites
      << ";ownership-arc=" << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ";ready=" << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

inline std::string BuildSymbolGraphScopeResolutionHandoffKey(
    const Objc3FrontendSymbolGraphScopeResolutionSummary &summary) {
  std::ostringstream out;
  out << "symbol_graph_nodes=" << summary.global_symbol_nodes << ":"
      << summary.function_symbol_nodes << ":" << summary.interface_symbol_nodes << ":"
      << summary.implementation_symbol_nodes << ":"
      << summary.interface_property_symbol_nodes << ":"
      << summary.implementation_property_symbol_nodes << ":"
      << summary.interface_method_symbol_nodes << ":"
      << summary.implementation_method_symbol_nodes
      << ";scope_surface=" << summary.top_level_scope_symbols << ":"
      << summary.nested_scope_symbols << ":" << summary.scope_frames_total
      << ";resolution_surface=" << summary.implementation_interface_resolution_sites
      << ":" << summary.implementation_interface_resolution_hits << ":"
      << summary.implementation_interface_resolution_misses << ":"
      << summary.method_resolution_sites << ":" << summary.method_resolution_hits << ":"
      << summary.method_resolution_misses
      << ";deterministic="
      << (summary.deterministic_symbol_graph_handoff ? "true" : "false") << ":"
      << (summary.deterministic_scope_resolution_handoff ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::pipeline::orchestration
