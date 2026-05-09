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
  summary.canonicalization_hint_sites = migration_hints.legacy_total();

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
    const Objc3FrontendOptions &options,
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &inventory_summary) {
  Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary summary;
  summary.language_profile = "canonical";
  summary.legacy_yes_sites = migration_hints.legacy_yes_count;
  summary.legacy_no_sites = migration_hints.legacy_no_count;
  summary.legacy_null_sites = migration_hints.legacy_null_count;
  summary.legacy_total_sites = migration_hints.legacy_total();
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

Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
BuildToolingDiagnosticTaxonomyPortabilityContractSummary(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &migration_summary,
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
        &diagnostic_surface) {
  Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary summary;
  summary.portability_dependency_count =
      summary.portability_dependency_surface_ids.size();
  summary.diagnostics_total = diagnostic_surface.diagnostics_total;
  summary.diagnostics_after_pass_final =
      diagnostic_surface.diagnostics_after_pass_final;
  summary.diagnostics_emitted_total =
      diagnostic_surface.diagnostics_emitted_total;
  summary.ownership_arc_diagnostic_candidate_sites =
      diagnostic_surface.ownership_arc_diagnostic_candidate_sites;
  summary.ownership_arc_fixit_available_sites =
      diagnostic_surface.ownership_arc_fixit_available_sites;
  summary.ownership_arc_profiled_sites =
      diagnostic_surface.ownership_arc_profiled_sites;
  summary.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      diagnostic_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  summary.ownership_arc_empty_fixit_hint_sites =
      diagnostic_surface.ownership_arc_empty_fixit_hint_sites;
  summary.ownership_arc_contract_violation_sites =
      diagnostic_surface.ownership_arc_contract_violation_sites;
  summary.migration_canonicalization_candidate_sites =
      migration_summary.canonicalization_candidate_sites;
  summary.migration_surface_ready =
      migration_summary.ready_for_semantic_expansion;
  summary.portability_dependencies_frozen =
      summary.portability_dependency_count == 11;
  summary.recovery_replay_key = diagnostic_surface.recovery_replay_key;
  summary.arc_diagnostics_fixit_replay_key =
      diagnostic_surface.arc_diagnostics_fixit_replay_key;
  summary.deterministic_handoff =
      summary.migration_surface_ready &&
      summary.portability_dependencies_frozen &&
      diagnostic_surface.core_feature_impl_ready &&
      diagnostic_surface.arc_diagnostics_fixit_summary_deterministic &&
      diagnostic_surface.arc_diagnostics_fixit_handoff_deterministic &&
      diagnostic_surface.arc_diagnostics_fixit_case_accounting_consistent &&
      diagnostic_surface.replay_keys_ready &&
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites +
              summary.ownership_arc_contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites +
              summary.ownership_arc_contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites +
              summary.ownership_arc_contract_violation_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.migration_surface_ready) {
    summary.failure_reason =
        "tooling migration/canonicalization source completion prerequisite is not ready";
  } else if (!diagnostic_surface.core_feature_impl_ready) {
    summary.failure_reason =
        "semantic diagnostic taxonomy/fix-it core feature implementation surface is not ready";
  }
  summary.replay_key =
      BuildToolingDiagnosticTaxonomyPortabilityContractReplayKey(summary);
  return summary;
}

Objc3ToolingFeatureSpecificFixitSynthesisSummary
BuildToolingFeatureSpecificFixitSynthesisSummary(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
        &taxonomy_summary) {
  Objc3ToolingFeatureSpecificFixitSynthesisSummary summary;
  summary.fixit_family_count = summary.fixit_family_ids.size();
  summary.migration_fixit_candidate_sites =
      taxonomy_summary.migration_canonicalization_candidate_sites;
  summary.migrator_candidate_sites =
      taxonomy_summary.migration_canonicalization_candidate_sites;
  summary.ownership_arc_fixit_available_sites =
      taxonomy_summary.ownership_arc_fixit_available_sites;
  summary.ownership_arc_empty_fixit_hint_sites =
      taxonomy_summary.ownership_arc_empty_fixit_hint_sites;
  summary.diagnostic_taxonomy_ready =
      taxonomy_summary.ready_for_lowering_and_runtime;
  summary.deterministic_handoff =
      summary.diagnostic_taxonomy_ready && summary.fixit_family_count == 2 &&
      summary.migration_fixit_candidate_sites ==
          summary.migrator_candidate_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.diagnostic_taxonomy_ready) {
    summary.failure_reason =
        "tooling diagnostic taxonomy and portability contract prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingFeatureSpecificFixitSynthesisReplayKey(summary);
  return summary;
}

std::string BuildSymbolGraphScopeResolutionHandoffKey(
    const Objc3FrontendSymbolGraphScopeResolutionSummary &summary) {
  std::ostringstream out;
  out << "symbol_graph_nodes="
      << summary.global_symbol_nodes << ":" << summary.function_symbol_nodes << ":" << summary.interface_symbol_nodes
      << ":" << summary.implementation_symbol_nodes << ":" << summary.interface_property_symbol_nodes << ":"
      << summary.implementation_property_symbol_nodes << ":" << summary.interface_method_symbol_nodes << ":"
      << summary.implementation_method_symbol_nodes
      << ";scope_surface="
      << summary.top_level_scope_symbols << ":" << summary.nested_scope_symbols << ":"
      << summary.scope_frames_total
      << ";resolution_surface="
      << summary.implementation_interface_resolution_sites << ":" << summary.implementation_interface_resolution_hits
      << ":" << summary.implementation_interface_resolution_misses << ":" << summary.method_resolution_sites << ":"
      << summary.method_resolution_hits << ":" << summary.method_resolution_misses
      << ";deterministic="
      << (summary.deterministic_symbol_graph_handoff ? "true" : "false") << ":"
      << (summary.deterministic_scope_resolution_handoff ? "true" : "false");
  return out.str();
}

Objc3FrontendSymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummary(
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendSymbolGraphScopeResolutionSummary summary;
  const Objc3SymbolGraphScopeResolutionSummary &integration_summary =
      integration_surface.symbol_graph_scope_resolution_summary;
  const Objc3SymbolGraphScopeResolutionSummary &type_metadata_summary =
      type_metadata_handoff.symbol_graph_scope_resolution_summary;

  const auto select_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.global_symbol_nodes = select_value(integration_summary.global_symbol_nodes,
                                             type_metadata_summary.global_symbol_nodes);
  summary.function_symbol_nodes = select_value(integration_summary.function_symbol_nodes,
                                               type_metadata_summary.function_symbol_nodes);
  summary.interface_symbol_nodes = select_value(integration_summary.interface_symbol_nodes,
                                                type_metadata_summary.interface_symbol_nodes);
  summary.implementation_symbol_nodes = select_value(integration_summary.implementation_symbol_nodes,
                                                     type_metadata_summary.implementation_symbol_nodes);
  summary.interface_property_symbol_nodes = select_value(integration_summary.interface_property_symbol_nodes,
                                                         type_metadata_summary.interface_property_symbol_nodes);
  summary.implementation_property_symbol_nodes = select_value(integration_summary.implementation_property_symbol_nodes,
                                                              type_metadata_summary.implementation_property_symbol_nodes);
  summary.interface_method_symbol_nodes = select_value(integration_summary.interface_method_symbol_nodes,
                                                       type_metadata_summary.interface_method_symbol_nodes);
  summary.implementation_method_symbol_nodes = select_value(integration_summary.implementation_method_symbol_nodes,
                                                            type_metadata_summary.implementation_method_symbol_nodes);
  summary.top_level_scope_symbols = select_value(integration_summary.top_level_scope_symbols,
                                                 type_metadata_summary.top_level_scope_symbols);
  summary.nested_scope_symbols = select_value(integration_summary.nested_scope_symbols,
                                              type_metadata_summary.nested_scope_symbols);
  summary.scope_frames_total = select_value(integration_summary.scope_frames_total,
                                            type_metadata_summary.scope_frames_total);
  summary.implementation_interface_resolution_sites =
      select_value(integration_summary.implementation_interface_resolution_sites,
                   type_metadata_summary.implementation_interface_resolution_sites);
  summary.implementation_interface_resolution_hits =
      select_value(integration_summary.implementation_interface_resolution_hits,
                   type_metadata_summary.implementation_interface_resolution_hits);
  summary.implementation_interface_resolution_misses =
      select_value(integration_summary.implementation_interface_resolution_misses,
                   type_metadata_summary.implementation_interface_resolution_misses);
  summary.method_resolution_sites = select_value(integration_summary.method_resolution_sites,
                                                 type_metadata_summary.method_resolution_sites);
  summary.method_resolution_hits = select_value(integration_summary.method_resolution_hits,
                                                type_metadata_summary.method_resolution_hits);
  summary.method_resolution_misses = select_value(integration_summary.method_resolution_misses,
                                                  type_metadata_summary.method_resolution_misses);

  const bool symbol_graph_fields_match =
      integration_summary.global_symbol_nodes == type_metadata_summary.global_symbol_nodes &&
      integration_summary.function_symbol_nodes == type_metadata_summary.function_symbol_nodes &&
      integration_summary.interface_symbol_nodes == type_metadata_summary.interface_symbol_nodes &&
      integration_summary.implementation_symbol_nodes == type_metadata_summary.implementation_symbol_nodes &&
      integration_summary.interface_property_symbol_nodes == type_metadata_summary.interface_property_symbol_nodes &&
      integration_summary.implementation_property_symbol_nodes ==
          type_metadata_summary.implementation_property_symbol_nodes &&
      integration_summary.interface_method_symbol_nodes == type_metadata_summary.interface_method_symbol_nodes &&
      integration_summary.implementation_method_symbol_nodes ==
          type_metadata_summary.implementation_method_symbol_nodes;
  const bool scope_resolution_fields_match =
      integration_summary.top_level_scope_symbols == type_metadata_summary.top_level_scope_symbols &&
      integration_summary.nested_scope_symbols == type_metadata_summary.nested_scope_symbols &&
      integration_summary.scope_frames_total == type_metadata_summary.scope_frames_total &&
      integration_summary.implementation_interface_resolution_sites ==
          type_metadata_summary.implementation_interface_resolution_sites &&
      integration_summary.implementation_interface_resolution_hits ==
          type_metadata_summary.implementation_interface_resolution_hits &&
      integration_summary.implementation_interface_resolution_misses ==
          type_metadata_summary.implementation_interface_resolution_misses &&
      integration_summary.method_resolution_sites == type_metadata_summary.method_resolution_sites &&
      integration_summary.method_resolution_hits == type_metadata_summary.method_resolution_hits &&
      integration_summary.method_resolution_misses == type_metadata_summary.method_resolution_misses;

  summary.deterministic_symbol_graph_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      symbol_graph_fields_match &&
      summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols;
  summary.deterministic_scope_resolution_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      scope_resolution_fields_match &&
      summary.resolution_hits_total() <= summary.resolution_sites_total() &&
      summary.resolution_hits_total() + summary.resolution_misses_total() == summary.resolution_sites_total();
  summary.deterministic_handoff_key = BuildSymbolGraphScopeResolutionHandoffKey(summary);
  return summary;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  std::vector<Objc3LexToken> tokens =
      RunObjc3FrontendLexStage(source, options, result);
  RunObjc3FrontendParseStageIfLexClean(tokens, result);
  result.selector_normalization_summary =
      BuildSelectorNormalizationSummary(Objc3ParsedProgramAst(result.program));
  result.property_attribute_summary =
      BuildPropertyAttributeSummary(Objc3ParsedProgramAst(result.program));
  result.object_pointer_nullability_generics_summary =
      BuildObjectPointerNullabilityGenericsSummary(Objc3ParsedProgramAst(result.program));
  result.type_system_type_source_closure_summary = BuildTypeSystemTypeSourceClosureSummary(
      Objc3ParsedProgramAst(result.program),
      result.object_pointer_nullability_generics_summary);
  result.control_flow_control_flow_source_closure_summary =
      BuildControlFlowControlFlowSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.error_handling_error_source_closure_summary =
      BuildErrorHandlingErrorSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.concurrency_async_source_closure_summary =
      BuildConcurrencyAsyncSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.ownership_system_extension_source_closure_summary =
      BuildOwnershipSystemExtensionSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_cleanup_resource_capture_source_completion_summary =
      BuildOwnershipCleanupResourceCaptureSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_retainable_c_family_source_completion_summary =
      BuildOwnershipRetainableCFamilySourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_closure_summary =
      BuildDispatchDispatchIntentSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_completion_summary =
      BuildDispatchDispatchIntentSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_metaprogramming_source_closure_summary =
      BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_macro_package_provenance_source_completion_summary =
      BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_property_behavior_source_completion_summary =
      BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_foreign_import_source_closure_summary =
      BuildInteropForeignImportSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_cpp_swift_interop_annotation_source_completion_summary =
      BuildInteropCppSwiftInteropAnnotationSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_actor_member_isolation_source_closure_summary =
      BuildConcurrencyActorMemberIsolationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_task_group_cancellation_source_closure_summary =
      BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.tooling_diagnostics_migrator_source_inventory_summary =
      BuildToolingDiagnosticsMigratorSourceInventorySummary(
          result.migration_hints, result.error_handling_error_source_closure_summary,
          result.concurrency_async_source_closure_summary,
          result.concurrency_actor_member_isolation_source_closure_summary,
          result.concurrency_task_group_cancellation_source_closure_summary,
          result.ownership_system_extension_source_closure_summary,
          result.ownership_cleanup_resource_capture_source_completion_summary,
          result.ownership_retainable_c_family_source_completion_summary,
          result.dispatch_dispatch_intent_source_closure_summary,
          result.dispatch_dispatch_intent_source_completion_summary,
          result.metaprogramming_metaprogramming_source_closure_summary,
          result.metaprogramming_macro_package_provenance_source_completion_summary,
          result.metaprogramming_property_behavior_source_completion_summary,
          result.interop_foreign_import_source_closure_summary,
          result.interop_cpp_swift_interop_annotation_source_completion_summary);
  result.tooling_migration_canonicalization_source_completion_summary =
      BuildToolingMigrationCanonicalizationSourceCompletionSummary(
          options, result.migration_hints,
          result.tooling_diagnostics_migrator_source_inventory_summary);
  result.protocol_category_summary =
      BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                   result.integration_surface,
                                   result.sema_type_metadata_handoff);
  result.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,
                                               result.protocol_category_summary,
                                               result.integration_surface,
                                               result.sema_type_metadata_handoff);
  result.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                             result.sema_type_metadata_handoff);
  const bool allow_error_handling_error_runtime_surface = true;
  if (ShouldRunObjc3FrontendSemaStage(result)) {
    Objc3SemaPassManagerResult sema_result = RunObjc3FrontendSemaStage(
        result, options, allow_error_handling_error_runtime_surface);
    result.integration_surface = std::move(sema_result.integration_surface);
    result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);
    result.protocol_category_summary =
        BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                     result.integration_surface,
                                     result.sema_type_metadata_handoff);
    result.class_protocol_category_linking_summary =
        BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,
                                                 result.protocol_category_summary,
                                                 result.integration_surface,
                                                 result.sema_type_metadata_handoff);
    result.symbol_graph_scope_resolution_summary =
        BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                               result.sema_type_metadata_handoff);
    result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;
    result.sema_pass_flow_summary = sema_result.sema_pass_flow_summary;
    result.sema_parity_surface = sema_result.parity_surface;
  }
  result.control_flow_control_flow_semantic_model_summary =
      BuildControlFlowControlFlowSemanticModelSummary(
          Objc3ParsedProgramAst(result.program));
  result.error_handling_error_semantic_model_summary =
      BuildErrorHandlingErrorSemanticModelSummary(
          result.error_handling_error_source_closure_summary, result.integration_surface);
  result.concurrency_actor_isolation_sendable_semantic_model_summary =
      BuildConcurrencyActorIsolationSendableSemanticModelSummary(
          result.concurrency_actor_member_isolation_source_closure_summary,
          result.integration_surface);
  result.concurrency_actor_isolation_sendability_enforcement_summary =
      BuildConcurrencyActorIsolationSendabilityEnforcementSummary(
          Objc3ParsedProgramAst(result.program),
          result.concurrency_actor_isolation_sendable_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_actor_race_hazard_escape_diagnostics_summary =
      BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
          Objc3ParsedProgramAst(result.program),
          result.concurrency_actor_isolation_sendability_enforcement_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_task_executor_cancellation_semantic_model_summary =
      BuildConcurrencyTaskExecutorCancellationSemanticModelSummary(
          result.concurrency_task_group_cancellation_source_closure_summary,
          result.integration_surface);
  result.ownership_system_extension_semantic_model_summary =
      BuildOwnershipSystemExtensionSemanticModelSummary(
          result.ownership_system_extension_source_closure_summary,
          result.ownership_cleanup_resource_capture_source_completion_summary,
          result.ownership_retainable_c_family_source_completion_summary);
  result.metaprogramming_expansion_behavior_semantic_model_summary =
      BuildMetaprogrammingExpansionBehaviorSemanticModelSummary(
          result.metaprogramming_metaprogramming_source_closure_summary,
          result.metaprogramming_macro_package_provenance_source_completion_summary,
          result.metaprogramming_property_behavior_source_completion_summary,
          result.stage_diagnostics.semantic);
  result.metaprogramming_derive_expansion_inventory_summary =
      BuildMetaprogrammingDeriveExpansionInventorySummary(
          result.program.ast,
          result.metaprogramming_expansion_behavior_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.metaprogramming_macro_safety_sandbox_determinism_summary =
      BuildMetaprogrammingMacroSafetySandboxDeterminismSummary(
          result.program.ast,
          result.metaprogramming_derive_expansion_inventory_summary,
          result.stage_diagnostics.semantic);
  result.metaprogramming_property_behavior_legality_compatibility_summary =
      BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
          result.program.ast,
          result.metaprogramming_macro_safety_sandbox_determinism_summary,
          result.stage_diagnostics.semantic);
  result.dispatch_dispatch_intent_semantic_model_summary =
      BuildDispatchDispatchIntentSemanticModelSummary(
          result.dispatch_dispatch_intent_source_completion_summary,
          result.integration_surface);
  result.dispatch_dispatch_intent_legality_summary =
      BuildDispatchDispatchIntentLegalitySummary(
          Objc3ParsedProgramAst(result.program),
          result.dispatch_dispatch_intent_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.dispatch_dispatch_intent_compatibility_summary =
      BuildDispatchDispatchIntentCompatibilitySummary(
          Objc3ParsedProgramAst(result.program),
          result.dispatch_dispatch_intent_legality_summary,
          result.stage_diagnostics.semantic);
  result.ownership_resource_move_use_after_move_semantics_summary =
      BuildOwnershipResourceMoveUseAfterMoveSemanticsSummary(
          Objc3ParsedProgramAst(result.program),
          result.ownership_system_extension_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.ownership_borrowed_pointer_escape_analysis_summary =
      BuildOwnershipBorrowedPointerEscapeAnalysisSummary(
          Objc3ParsedProgramAst(result.program),
          result.ownership_resource_move_use_after_move_semantics_summary,
          result.stage_diagnostics.semantic);
  result.ownership_capture_list_retainable_family_legality_completion_summary =
      BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
          Objc3ParsedProgramAst(result.program),
          result.ownership_borrowed_pointer_escape_analysis_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_structured_task_cancellation_semantic_summary =
      BuildConcurrencyStructuredTaskCancellationSemanticSummary(
          result.concurrency_task_executor_cancellation_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_executor_hop_affinity_compatibility_summary =
      BuildConcurrencyExecutorHopAffinityCompatibilitySummary(
          result.concurrency_structured_task_cancellation_semantic_summary,
          result.concurrency_async_source_closure_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_async_effect_suspension_semantic_model_summary =
      BuildConcurrencyAsyncEffectSuspensionSemanticModelSummary(
          result.concurrency_async_source_closure_summary, result.integration_surface);
  result.concurrency_await_suspension_resume_semantic_summary =
      BuildConcurrencyAwaitSuspensionResumeSemanticSummary(
          result.concurrency_async_effect_suspension_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.concurrency_async_diagnostics_compatibility_summary =
      BuildConcurrencyAsyncDiagnosticsCompatibilitySummary(
          result.concurrency_await_suspension_resume_semantic_summary,
          result.concurrency_async_source_closure_summary,
          Objc3ParsedProgramAst(result.program),
          result.stage_diagnostics.semantic);
  result.error_handling_try_do_catch_semantic_summary =
      BuildErrorHandlingTryDoCatchSemanticSummary(
          Objc3ParsedProgramAst(result.program),
          result.integration_surface,
          allow_error_handling_error_runtime_surface,
          result.stage_diagnostics.semantic);
  result.error_handling_error_bridge_legality_summary =
      BuildErrorHandlingErrorBridgeLegalitySummary(
          Objc3ParsedProgramAst(result.program),
          allow_error_handling_error_runtime_surface,
          result.stage_diagnostics.semantic);
  result.interop_interop_semantic_model_summary =
      BuildInteropInteropSemanticModelSummary(
          result.interop_foreign_import_source_closure_summary,
          result.interop_cpp_swift_interop_annotation_source_completion_summary,
          result.ownership_capture_list_retainable_family_legality_completion_summary,
          result.error_handling_error_bridge_legality_summary,
          result.concurrency_async_diagnostics_compatibility_summary,
          result.concurrency_actor_race_hazard_escape_diagnostics_summary);
  result.effects_ownership_semantic_model_summary =
      BuildEffectsOwnershipSemanticModelSummary(
          result.integration_surface,
          result.error_handling_error_semantic_model_summary,
          result.concurrency_async_effect_suspension_semantic_model_summary,
          result.concurrency_task_executor_cancellation_semantic_model_summary,
          result.concurrency_actor_isolation_sendable_semantic_model_summary,
          result.interop_interop_semantic_model_summary);
  result.cross_module_semantic_contracts_diagnostics_summary =
      BuildCrossModuleSemanticContractsDiagnosticsSummary(
          result.integration_surface,
          result.interop_interop_semantic_model_summary);
  result.interop_interop_runtime_parity_summary =
      BuildInteropInteropRuntimeParitySummary(
          Objc3ParsedProgramAst(result.program),
          result.interop_interop_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.interop_cpp_interop_interaction_summary =
      BuildInteropCppInteropInteractionSummary(
          Objc3ParsedProgramAst(result.program),
          result.interop_interop_runtime_parity_summary,
          result.stage_diagnostics.semantic);
  result.interop_swift_interop_isolation_summary =
      BuildInteropSwiftInteropIsolationSummary(
          Objc3ParsedProgramAst(result.program),
          result.interop_cpp_interop_interaction_summary,
          result.stage_diagnostics.semantic);
  result.runtime_metadata_source_records =
      BuildRuntimeMetadataSourceRecordSet(Objc3ParsedProgramAst(result.program));
  result.executable_metadata_source_graph = BuildExecutableMetadataSourceGraph(
      Objc3ParsedProgramAst(result.program),
      result.runtime_metadata_source_records);
  result.executable_metadata_semantic_consistency_boundary =
      BuildExecutableMetadataSemanticConsistencyBoundary(
          result.executable_metadata_source_graph,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary);
  result.executable_metadata_semantic_validation_surface =
      BuildExecutableMetadataSemanticValidationSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.sema_type_metadata_handoff,
          result.class_protocol_category_linking_summary);
  result.executable_metadata_lowering_handoff_surface =
      BuildExecutableMetadataLoweringHandoffSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.sema_type_metadata_handoff,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary,
          result.sema_parity_surface);
  result.executable_metadata_typed_lowering_handoff =
      BuildExecutableMetadataTypedLoweringHandoff(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.executable_metadata_lowering_handoff_surface);
  result.runtime_metadata_source_ownership_boundary =
      BuildRuntimeMetadataSourceOwnershipBoundary(result.runtime_metadata_source_records,
                                                  result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary = BuildRuntimeExportLegalityBoundary(
      result.runtime_metadata_source_ownership_boundary,
      result.typed_sema_to_lowering_contract_surface,
      result.integration_surface,
      result.protocol_category_summary,
      result.class_protocol_category_linking_summary,
      result.selector_normalization_summary,
      result.property_attribute_summary,
      result.object_pointer_nullability_generics_summary,
      result.symbol_graph_scope_resolution_summary,
      result.sema_parity_surface);
  result.runtime_export_enforcement_summary =
      BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
  if (result.stage_diagnostics.semantic.empty() &&
      HasRuntimeMetadataSourceRecords(result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    const std::vector<Objc3RuntimeExportBlockingDiagnostic>
        runtime_export_blocking_diagnostics =
            BuildRuntimeExportBlockingDiagnostics(
                result.runtime_metadata_source_records,
                result.runtime_export_enforcement_summary);
    if (!runtime_export_blocking_diagnostics.empty()) {
      for (const auto &diagnostic : runtime_export_blocking_diagnostics) {
        result.stage_diagnostics.semantic.push_back(
            MakeDiag(diagnostic.line, diagnostic.column, diagnostic.code,
                     diagnostic.message));
      }
    } else {
      std::string runtime_export_failure_reason =
          result.runtime_export_enforcement_summary.failure_reason;
      if (runtime_export_failure_reason ==
              "runtime metadata export shape drift detected before lowering" &&
          !result.runtime_export_legality_boundary.failure_reason.empty()) {
        runtime_export_failure_reason +=
            " (" + result.runtime_export_legality_boundary.failure_reason + ")";
      }
      result.stage_diagnostics.semantic.push_back(MakeDiag(
          result.runtime_export_enforcement_summary.first_failure_line,
          result.runtime_export_enforcement_summary.first_failure_column,
          "O3S260",
          "runtime metadata export blocked: " + runtime_export_failure_reason));
    }
  }
  result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface,
          result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold);
  result.tooling_diagnostic_taxonomy_portability_contract_summary =
      BuildToolingDiagnosticTaxonomyPortabilityContractSummary(
          result.tooling_migration_canonicalization_source_completion_summary,
          result
              .semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface);
  result.tooling_feature_specific_fixit_synthesis_summary =
      BuildToolingFeatureSpecificFixitSynthesisSummary(
          result.tooling_diagnostic_taxonomy_portability_contract_summary);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(
          result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface,
          result.parse_lowering_readiness_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_spec_delta_closure_scaffold =
      BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_core_feature_implementation_surface =
      BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          result.semantic_stability_spec_delta_closure_scaffold);
  result.lowering_runtime_stability_invariant_scaffold =
      BuildObjc3LoweringRuntimeStabilityInvariantScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.lowering_pipeline_pass_graph_scaffold =
      BuildObjc3LoweringPipelinePassGraphScaffold(result, options);
  result.lowering_pipeline_pass_graph_core_feature_surface =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(result, options);
  result.ir_emission_completeness_scaffold =
      BuildObjc3IREmissionCompletenessScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_scaffold =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
          result);
  result.lowering_runtime_stability_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          result.lowering_runtime_stability_invariant_scaffold);
  TransportObjc3FrontendPipelineDiagnostics(result);
  return result;
}
