#include "pipeline/frontend_pipeline_orchestration_owners.h"

#include "parse/objc3_ast_builder_contract.h"
#include "sema/objc3_semantic_passes.h"

namespace objc3_frontend_pipeline_orchestration {

void PopulateSemanticModelSummaries(
    Objc3FrontendPipelineResult &result,
    bool allow_error_handling_error_runtime_surface) {
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
}

}  // namespace objc3_frontend_pipeline_orchestration
