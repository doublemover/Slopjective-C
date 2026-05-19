#include "pipeline/frontend_tooling_source_completion_helpers.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
BuildToolingDiagnosticsMigratorSourceInventorySummary(
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &error_handling_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &concurrency_async_summary,
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &concurrency_actor_summary,
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &concurrency_task_summary,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &ownership_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &ownership_cleanup_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_family_summary,
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary
        &dispatch_closure_summary,
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &dispatch_completion_summary,
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &metaprogramming_closure_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &metaprogramming_macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &metaprogramming_property_summary,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_completion_summary) {
  Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary summary;

  summary.advanced_feature_family_count = 6;
  summary.dependency_surface_count = summary.dependency_contract_ids.size();
  summary.aggregated_source_only_claim_count =
      error_handling_summary.source_only_claim_ids.size() +
      concurrency_async_summary.source_only_claim_ids.size() +
      concurrency_actor_summary.source_only_claim_ids.size() +
      concurrency_task_summary.source_only_claim_ids.size() +
      ownership_summary.source_only_claim_ids.size() +
      ownership_cleanup_summary.source_only_claim_ids.size() +
      ownership_family_summary.source_only_claim_ids.size() +
      dispatch_closure_summary.source_only_claim_ids.size() +
      dispatch_completion_summary.source_only_claim_ids.size() +
      metaprogramming_closure_summary.source_only_claim_ids.size() +
      metaprogramming_macro_summary.source_only_claim_ids.size() +
      metaprogramming_property_summary.source_only_claim_ids.size() +
      interop_closure_summary.source_only_claim_ids.size() +
      interop_completion_summary.source_only_claim_ids.size();
  summary.fail_closed_construct_count =
      error_handling_summary.fail_closed_construct_ids.size();
  summary.error_surface_sites =
      error_handling_summary.function_throws_declaration_sites +
      error_handling_summary.method_throws_declaration_sites +
      error_handling_summary.result_like_sites + error_handling_summary.result_payload_sites +
      error_handling_summary.ns_error_bridging_sites +
      error_handling_summary.ns_error_out_parameter_sites +
      error_handling_summary.ns_error_bridge_path_sites +
      error_handling_summary.objc_nserror_attribute_sites +
      error_handling_summary.objc_status_code_attribute_sites +
      error_handling_summary.try_keyword_sites + error_handling_summary.throw_keyword_sites +
      error_handling_summary.catch_keyword_sites;
  summary.concurrency_surface_sites =
      concurrency_async_summary.async_keyword_sites +
      concurrency_async_summary.await_keyword_sites +
      concurrency_async_summary.executor_attribute_sites +
      concurrency_actor_summary.actor_interface_sites +
      concurrency_actor_summary.actor_method_sites +
      concurrency_actor_summary.actor_property_sites +
      concurrency_actor_summary.objc_nonisolated_annotation_sites +
      concurrency_actor_summary.actor_member_executor_annotation_sites +
      concurrency_task_summary.task_creation_sites +
      concurrency_task_summary.task_group_scope_sites +
      concurrency_task_summary.task_group_add_task_sites +
      concurrency_task_summary.task_group_wait_next_sites +
      concurrency_task_summary.task_group_cancel_all_sites +
      concurrency_task_summary.cancellation_check_sites +
      concurrency_task_summary.cancellation_handler_sites;
  summary.system_surface_sites =
      ownership_summary.resource_attribute_sites +
      ownership_summary.resource_close_clause_sites +
      ownership_summary.borrowed_pointer_sites +
      ownership_summary.returns_borrowed_attribute_sites +
      ownership_summary.explicit_capture_list_sites +
      ownership_summary.explicit_capture_item_sites +
      ownership_cleanup_summary.cleanup_attribute_sites +
      ownership_cleanup_summary.cleanup_sugar_sites +
      ownership_cleanup_summary.resource_sugar_sites +
      ownership_family_summary.family_retain_sites +
      ownership_family_summary.family_release_sites +
      ownership_family_summary.family_autorelease_sites +
      ownership_family_summary.compatibility_returns_retained_sites +
      ownership_family_summary.compatibility_returns_not_retained_sites +
      ownership_family_summary.compatibility_consumed_sites;
  summary.dispatch_surface_sites =
      dispatch_closure_summary.direct_callable_sites +
      dispatch_closure_summary.final_callable_sites +
      dispatch_closure_summary.dynamic_callable_sites +
      dispatch_closure_summary.direct_members_container_sites +
      dispatch_closure_summary.final_container_sites +
      dispatch_closure_summary.sealed_container_sites +
      dispatch_completion_summary.prefixed_container_attribute_sites +
      dispatch_completion_summary.effective_direct_member_sites +
      dispatch_completion_summary.direct_members_defaulted_method_sites +
      dispatch_completion_summary.direct_members_dynamic_opt_out_sites;
  summary.metaprogramming_surface_sites =
      metaprogramming_closure_summary.derive_marker_sites +
      metaprogramming_closure_summary.macro_marker_sites +
      metaprogramming_closure_summary.property_behavior_sites +
      metaprogramming_macro_summary.macro_package_sites +
      metaprogramming_macro_summary.macro_provenance_sites +
      metaprogramming_macro_summary.expansion_visible_macro_sites +
      metaprogramming_property_summary.property_behavior_sites +
      metaprogramming_property_summary.synthesized_binding_visible_sites +
      metaprogramming_property_summary.synthesized_getter_visible_sites +
      metaprogramming_property_summary.synthesized_setter_visible_sites;
  summary.interop_surface_sites =
      interop_closure_summary.foreign_callable_sites +
      interop_closure_summary.import_module_annotation_sites +
      interop_closure_summary.imported_module_name_sites +
      interop_closure_summary.export_header_annotation_sites +
      interop_closure_summary.export_header_name_sites +
      interop_closure_summary.mixed_image_annotation_sites +
      interop_closure_summary.mixed_image_name_sites +
      interop_closure_summary.package_entry_annotation_sites +
      interop_closure_summary.package_entry_name_sites +
      interop_completion_summary.swift_name_annotation_sites +
      interop_completion_summary.swift_private_annotation_sites +
      interop_completion_summary.cpp_name_annotation_sites +
      interop_completion_summary.header_name_annotation_sites +
      interop_completion_summary.abi_alignment_annotation_sites +
      interop_completion_summary.foreign_type_annotation_sites +
      interop_completion_summary.named_annotation_payload_sites;
  summary.diagnostic_surface_sites =
      summary.error_surface_sites + summary.concurrency_surface_sites +
      summary.system_surface_sites + summary.dispatch_surface_sites +
      summary.metaprogramming_surface_sites + summary.interop_surface_sites;
  summary.fixit_surface_sites = summary.diagnostic_surface_sites;
  summary.migrator_surface_sites = summary.diagnostic_surface_sites;
  summary.canonicalization_hint_sites =
      canonical_literal_rejection_counts.total_literal_sites();

  const bool dependencies_ready =
      error_handling_summary.ready_for_semantic_expansion &&
      concurrency_async_summary.ready_for_semantic_expansion &&
      concurrency_actor_summary.ready_for_semantic_expansion &&
      concurrency_task_summary.ready_for_semantic_expansion &&
      ownership_summary.ready_for_semantic_expansion &&
      ownership_cleanup_summary.ready_for_semantic_expansion &&
      ownership_family_summary.ready_for_semantic_expansion &&
      dispatch_closure_summary.ready_for_semantic_expansion &&
      dispatch_completion_summary.ready_for_semantic_expansion &&
      metaprogramming_closure_summary.ready_for_semantic_expansion &&
      metaprogramming_macro_summary.ready_for_semantic_expansion &&
      metaprogramming_property_summary.ready_for_semantic_expansion &&
      interop_closure_summary.ready_for_semantic_expansion &&
      interop_completion_summary.ready_for_semantic_expansion;
  summary.diagnostics_inventory_source_supported = dependencies_ready;
  summary.fixit_inventory_source_supported = dependencies_ready;
  summary.migrator_inventory_source_supported = dependencies_ready;
  summary.deterministic_handoff =
      dependencies_ready && summary.dependency_surface_count == 14 &&
      summary.aggregated_source_only_claim_count > 0 &&
      summary.advanced_feature_family_count == 6 &&
      summary.diagnostic_surface_sites >= summary.error_surface_sites &&
      summary.diagnostic_surface_sites >= summary.interop_surface_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildToolingDiagnosticsMigratorSourceInventoryReplayKey(summary);
  return summary;
}

Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
BuildToolingMigrationCanonicalizationSourceCompletionSummary(
    const Objc3FrontendOptions &,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &inventory_summary) {
  Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary summary;
  summary.language_profile = "canonical";
  summary.legacy_yes_sites =
      canonical_literal_rejection_counts.yes_literal_sites;
  summary.legacy_no_sites =
      canonical_literal_rejection_counts.no_literal_sites;
  summary.legacy_null_sites =
      canonical_literal_rejection_counts.null_literal_sites;
  summary.legacy_total_sites =
      canonical_literal_rejection_counts.total_literal_sites();
  summary.canonical_true_rewrite_sites = summary.legacy_yes_sites;
  summary.canonical_false_rewrite_sites = summary.legacy_no_sites;
  summary.canonical_nil_rewrite_sites = summary.legacy_null_sites;
  summary.canonicalization_candidate_sites = summary.legacy_total_sites;
  summary.fixit_candidate_sites = summary.legacy_total_sites;
  summary.migrator_candidate_sites = summary.legacy_total_sites;
  summary.dependency_inventory_ready =
      inventory_summary.ready_for_semantic_expansion;
  summary.canonicalization_surface_supported = summary.dependency_inventory_ready;
  summary.fixit_migration_surface_supported = false;
  const bool counts_consistent =
      summary.legacy_total_sites ==
          summary.legacy_yes_sites + summary.legacy_no_sites +
              summary.legacy_null_sites &&
      summary.canonicalization_candidate_sites ==
          summary.canonical_true_rewrite_sites +
              summary.canonical_false_rewrite_sites +
              summary.canonical_nil_rewrite_sites &&
      summary.fixit_candidate_sites == summary.canonicalization_candidate_sites &&
      summary.migrator_candidate_sites ==
          summary.canonicalization_candidate_sites;
  summary.deterministic_handoff =
      summary.dependency_inventory_ready && counts_consistent &&
      inventory_summary.canonicalization_hint_sites ==
          summary.canonicalization_candidate_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  if (!summary.dependency_inventory_ready) {
    summary.failure_reason =
        "tooling diagnostics/fix-it/migrator inventory prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingMigrationCanonicalizationSourceCompletionReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
