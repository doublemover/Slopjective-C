#include "pipeline/objc3_frontend_pipeline.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_parse_support.h"
#include "pipeline/dispatch_surface_classification.h"
#include "pipeline/frontend_concurrency_source_closure_helpers.h"
#include "pipeline/frontend_control_flow_source_closure_helpers.h"
#include "pipeline/frontend_dispatch_source_completion_helpers.h"
#include "pipeline/frontend_error_handling_source_closure_helpers.h"
#include "pipeline/frontend_executable_metadata_handoff.h"
#include "pipeline/frontend_executable_metadata_source_graph_helpers.h"
#include "pipeline/frontend_executable_metadata_semantic_surface_helpers.h"
#include "pipeline/frontend_interop_source_closure_helpers.h"
#include "pipeline/frontend_interop_source_completion_helpers.h"
#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "pipeline/frontend_metaprogramming_source_completion_helpers.h"
#include "pipeline/frontend_ownership_retainable_c_family_completion_helpers.h"
#include "pipeline/frontend_ownership_source_completion_helpers.h"
#include "pipeline/frontend_ownership_source_closure_helpers.h"
#include "pipeline/frontend_phase_publication_helpers.h"
#include "pipeline/frontend_pipeline_result_handoff.h"
#include "pipeline/frontend_pipeline_sema_stage_runner.h"
#include "pipeline/frontend_pipeline_stage_sequence.h"
#include "pipeline/frontend_pipeline_stage_runner.h"
#include "pipeline/frontend_runtime_export_enforcement_helpers.h"
#include "pipeline/frontend_runtime_metadata_boundary_helpers.h"
#include "pipeline/frontend_runtime_metadata_source_record_helpers.h"
#include "pipeline/frontend_semantic_metadata_summary_helpers.h"
#include "pipeline/frontend_tooling_source_completion_helpers.h"
#include "pipeline/frontend_type_system_source_closure_helpers.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_surface.h"
#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface.h"
#include "sema/objc3_semantic_passes.h"
#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"
#include "sema/objc3_sema_pass_manager.h"
#include "support/objc3_property_storage_profile_helpers.h"

using objc3c::parse::support::MakeDiag;

namespace {

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
  summary.deterministic_handoff_key =
      objc3c::pipeline::orchestration::BuildSymbolGraphScopeResolutionHandoffKey(summary);
  return summary;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  std::vector<Objc3LexToken> tokens =
      RunObjc3FrontendLexParseStageSequence(source, options, result);
  result.selector_normalization_summary =
      objc3c::pipeline::orchestration::BuildSelectorNormalizationSummary(
          Objc3ParsedProgramAst(result.program));
  result.property_attribute_summary =
      objc3c::pipeline::orchestration::BuildPropertyAttributeSummary(
          Objc3ParsedProgramAst(result.program));
  result.object_pointer_nullability_generics_summary =
      objc3c::pipeline::orchestration::BuildObjectPointerNullabilityGenericsSummary(
          Objc3ParsedProgramAst(result.program));
  result.type_system_type_source_closure_summary =
      objc3c::pipeline::orchestration::BuildTypeSystemTypeSourceClosureSummary(
          Objc3ParsedProgramAst(result.program),
          result.object_pointer_nullability_generics_summary);
  result.control_flow_control_flow_source_closure_summary =
      objc3c::pipeline::orchestration::BuildControlFlowControlFlowSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.error_handling_error_source_closure_summary =
      objc3c::pipeline::orchestration::BuildErrorHandlingErrorSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.concurrency_async_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyAsyncSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.ownership_system_extension_source_closure_summary =
      objc3c::pipeline::orchestration::BuildOwnershipSystemExtensionSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_cleanup_resource_capture_source_completion_summary =
      objc3c::pipeline::orchestration::BuildOwnershipCleanupResourceCaptureSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_retainable_c_family_source_completion_summary =
      objc3c::pipeline::orchestration::BuildOwnershipRetainableCFamilySourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_closure_summary =
      objc3c::pipeline::orchestration::BuildDispatchDispatchIntentSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_completion_summary =
      objc3c::pipeline::orchestration::BuildDispatchDispatchIntentSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_metaprogramming_source_closure_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_macro_package_provenance_source_completion_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_property_behavior_source_completion_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_foreign_import_source_closure_summary =
      objc3c::pipeline::orchestration::BuildInteropForeignImportSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_cpp_swift_interop_annotation_source_completion_summary =
      objc3c::pipeline::orchestration::BuildInteropCppSwiftInteropAnnotationSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_actor_member_isolation_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyActorMemberIsolationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_task_group_cancellation_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.tooling_diagnostics_migrator_source_inventory_summary =
      objc3c::pipeline::orchestration::BuildToolingDiagnosticsMigratorSourceInventorySummary(
          result.canonical_literal_rejection_counts,
          result.error_handling_error_source_closure_summary,
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
      objc3c::pipeline::orchestration::BuildToolingMigrationCanonicalizationSourceCompletionSummary(
          options, result.canonical_literal_rejection_counts,
          result.tooling_diagnostics_migrator_source_inventory_summary);
  result.protocol_category_summary =
      objc3c::pipeline::orchestration::BuildProtocolCategorySummary(
          Objc3ParsedProgramAst(result.program),
          result.integration_surface,
          result.sema_type_metadata_handoff);
  result.class_protocol_category_linking_summary =
      objc3c::pipeline::orchestration::BuildClassProtocolCategoryLinkingSummary(
          result.sema_type_metadata_handoff.interface_implementation_summary,
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
    AdoptObjc3FrontendSemaResult(result, std::move(sema_result));
    result.protocol_category_summary =
        objc3c::pipeline::orchestration::BuildProtocolCategorySummary(
            Objc3ParsedProgramAst(result.program),
            result.integration_surface,
            result.sema_type_metadata_handoff);
    result.class_protocol_category_linking_summary =
        objc3c::pipeline::orchestration::BuildClassProtocolCategoryLinkingSummary(
            result.sema_type_metadata_handoff.interface_implementation_summary,
            result.protocol_category_summary,
            result.integration_surface,
            result.sema_type_metadata_handoff);
    result.symbol_graph_scope_resolution_summary =
        BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                               result.sema_type_metadata_handoff);
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
      objc3c::pipeline::orchestration::BuildRuntimeMetadataSourceRecordSet(
          Objc3ParsedProgramAst(result.program));
  result.executable_metadata_source_graph =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSourceGraph(
          Objc3ParsedProgramAst(result.program),
          result.runtime_metadata_source_records);
  result.executable_metadata_semantic_consistency_boundary =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSemanticConsistencyBoundary(
          result.executable_metadata_source_graph,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary);
  result.executable_metadata_semantic_validation_surface =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSemanticValidationSurface(
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
      objc3c::pipeline::orchestration::BuildRuntimeMetadataSourceOwnershipBoundary(
          result.runtime_metadata_source_records,
          result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary =
      objc3c::pipeline::orchestration::BuildRuntimeExportLegalityBoundary(
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
      objc3c::pipeline::orchestration::BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
  if (result.stage_diagnostics.semantic.empty() &&
      objc3c::pipeline::orchestration::HasRuntimeMetadataSourceRecords(
          result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    const std::vector<
        objc3c::pipeline::orchestration::Objc3RuntimeExportBlockingDiagnostic>
        runtime_export_blocking_diagnostics =
            objc3c::pipeline::orchestration::BuildRuntimeExportBlockingDiagnostics(
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
  objc3c::pipeline::orchestration::AdoptObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
      result,
      objc3c::pipeline::orchestration::BuildObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
          result, options));
  objc3c::pipeline::orchestration::PopulateObjc3FrontendReadinessLoweringPhaseResult(
      result, options);
  TransportObjc3FrontendPipelineDiagnostics(result);
  return result;
}
