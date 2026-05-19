#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_actor_semantic_artifacts.h"
#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"
#include "artifacts/objc3_frontend_module_semantic_artifacts.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "artifacts/objc3_frontend_source_closure_artifacts.h"
#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceDispatchConcurrencyOwnershipFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context) {
  const auto &pipeline_result = context.pipeline_result;
  const auto &core_lowering_plan = context.core_lowering_plan;
  const auto &semantic_lowering_plan = context.semantic_lowering_plan;
  const auto &ownership_aware_lowering_plan =
      context.ownership_aware_lowering_plan;
  const auto &artifact_preservation_plan = context.artifact_preservation_plan;

  manifest
      << ",\"objc_dispatch_dynamism_and_dispatch_control_semantic_model\":"
      << BuildDispatchDispatchIntentSemanticModelSummaryJson(
             pipeline_result.dispatch_dispatch_intent_semantic_model_summary)
      << ",\"objc_dispatch_override_finality_and_sealing_legality\":"
      << BuildDispatchDispatchIntentLegalitySummaryJson(
             pipeline_result.dispatch_dispatch_intent_legality_summary)
      << ",\"objc_dispatch_dynamism_control_compatibility_diagnostics\":"
      << BuildDispatchDispatchIntentCompatibilitySummaryJson(
             pipeline_result.dispatch_dispatch_intent_compatibility_summary)
      << ",\"objc_dispatch_dispatch_control_lowering_contract\":"
      << BuildDispatchDispatchControlLoweringContractJson(
             pipeline_result.dispatch_dispatch_intent_semantic_model_summary,
             pipeline_result.dispatch_dispatch_intent_legality_summary,
             pipeline_result.dispatch_dispatch_intent_compatibility_summary,
             semantic_lowering_plan
                 .dispatch_dispatch_control_lowering_contract,
             semantic_lowering_plan
                 .dispatch_dispatch_control_lowering_replay_key)
      << ",\"objc_dispatch_dispatch_metadata_and_interface_preservation\":"
      << BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
             artifact_preservation_plan
                 .dispatch_dispatch_metadata_interface_preservation_summary)
      << ",\"objc_concurrency_actor_member_and_isolation_source_closure\":"
      << BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
             pipeline_result
                 .concurrency_actor_member_isolation_source_closure_summary)
      << ",\"objc_concurrency_actor_isolation_and_sendable_semantic_model\":"
      << BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
             pipeline_result
                 .concurrency_actor_isolation_sendable_semantic_model_summary)
      << ",\"objc_concurrency_actor_isolation_and_sendability_enforcement\":"
      << BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
             pipeline_result
                 .concurrency_actor_isolation_sendability_enforcement_summary)
      << ",\"objc_concurrency_actor_race_hazard_and_escape_diagnostics\":"
      << BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
             pipeline_result.concurrency_actor_race_hazard_escape_diagnostics_summary)
      << ",\"objc_concurrency_actor_lowering_and_metadata_contract\":"
      << BuildConcurrencyActorLoweringMetadataContractJson(
             pipeline_result
                 .concurrency_actor_member_isolation_source_closure_summary,
             pipeline_result
                 .concurrency_actor_isolation_sendability_enforcement_summary,
             pipeline_result.concurrency_actor_race_hazard_escape_diagnostics_summary,
             semantic_lowering_plan.concurrency_actor_lowering_metadata_contract,
             semantic_lowering_plan
                 .concurrency_actor_lowering_metadata_replay_key)
      << ",\"objc_concurrency_task_group_and_cancellation_source_closure\":"
      << BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
             pipeline_result
                 .concurrency_task_group_cancellation_source_closure_summary)
      << ",\"objc_concurrency_async_effect_and_suspension_semantic_model\":"
      << BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
             pipeline_result
                 .concurrency_async_effect_suspension_semantic_model_summary)
      << ",\"objc_concurrency_task_executor_and_cancellation_semantic_model\":"
      << BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
             pipeline_result
                 .concurrency_task_executor_cancellation_semantic_model_summary)
      << ",\"objc_ownership_system_extension_semantic_model\":"
      << BuildOwnershipSystemExtensionSemanticModelSummaryJson(
             pipeline_result.ownership_system_extension_semantic_model_summary)
      << ",\"objc_effects_ownership_semantic_model\":"
      << BuildEffectsOwnershipSemanticModelSummaryJson(
             pipeline_result.effects_ownership_semantic_model_summary)
      << ",\"objc_cross_module_semantic_contracts_and_diagnostics\":"
      << BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
             pipeline_result
                 .cross_module_semantic_contracts_diagnostics_summary)
      << ",\"objc_ownership_resource_move_and_use_after_move_semantics\":"
      << BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
             pipeline_result
                 .ownership_resource_move_use_after_move_semantics_summary)
      << ",\"objc_ownership_borrowed_pointer_escape_analysis\":"
      << BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
             pipeline_result.ownership_borrowed_pointer_escape_analysis_summary)
      << ",\"objc_ownership_capture_list_and_retainable_family_legality_completion\":"
      << BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
             pipeline_result
                 .ownership_capture_list_retainable_family_legality_completion_summary)
      << ",\"objc_ownership_system_extension_lowering_contract\":"
      << BuildOwnershipSystemExtensionLoweringContractJson(
             pipeline_result.ownership_system_extension_semantic_model_summary,
             pipeline_result
                 .ownership_resource_move_use_after_move_semantics_summary,
             pipeline_result.ownership_borrowed_pointer_escape_analysis_summary,
             pipeline_result
                 .ownership_capture_list_retainable_family_legality_completion_summary,
             semantic_lowering_plan
                 .ownership_system_extension_lowering_contract,
             semantic_lowering_plan
                 .ownership_system_extension_lowering_replay_key)
      << ",\"objc_ownership_borrowed_pointer_and_retainable_family_abi_completion\":"
      << BuildOwnershipBorrowedRetainableAbiCompletionJson(
             semantic_lowering_plan
                 .ownership_system_extension_lowering_contract,
             pipeline_result.ownership_system_extension_source_closure_summary,
             pipeline_result
                 .ownership_retainable_c_family_source_completion_summary,
             semantic_lowering_plan
                 .ownership_system_extension_lowering_replay_key,
             semantic_lowering_plan
                 .ownership_borrowed_retainable_abi_completion_replay_key)
      << ",\"objc_concurrency_structured_task_and_cancellation_semantics\":"
      << BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
             pipeline_result
                 .concurrency_structured_task_cancellation_semantic_summary)
      << ",\"objc_concurrency_executor_hop_and_affinity_compatibility_completion\":"
      << BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
             pipeline_result
                 .concurrency_executor_hop_affinity_compatibility_summary)
      << ",\"objc_concurrency_await_suspension_and_resume_semantics\":"
      << BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
             pipeline_result.concurrency_await_suspension_resume_semantic_summary)
      << ",\"objc_concurrency_async_diagnostics_and_compatibility_completion\":"
      << BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
             pipeline_result.concurrency_async_diagnostics_compatibility_summary)
      << ",\"objc_concurrency_continuation_abi_and_async_lowering_contract\":"
      << BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
             semantic_lowering_plan
                 .concurrency_async_continuation_lowering_contract,
             semantic_lowering_plan
                 .concurrency_await_lowering_suspension_state_lowering_contract,
             semantic_lowering_plan
                 .concurrency_async_continuation_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_await_lowering_suspension_state_lowering_replay_key)
      << ",\"objc_concurrency_task_runtime_lowering_contract\":"
      << BuildConcurrencyTaskRuntimeLoweringContractJson(
             pipeline_result
                 .concurrency_task_executor_cancellation_semantic_model_summary,
             pipeline_result
                 .concurrency_structured_task_cancellation_semantic_summary,
             pipeline_result
                 .concurrency_executor_hop_affinity_compatibility_summary,
             semantic_lowering_plan
                 .concurrency_actor_isolation_sendability_lowering_contract,
             semantic_lowering_plan
                 .concurrency_actor_isolation_sendability_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_task_runtime_interop_cancellation_lowering_contract,
             semantic_lowering_plan
                 .concurrency_task_runtime_interop_cancellation_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_concurrency_replay_race_guard_lowering_contract,
             semantic_lowering_plan
                 .concurrency_concurrency_replay_race_guard_lowering_replay_key)
      << ",\"objc_concurrency_task_group_and_runtime_abi_completion\":"
      << BuildConcurrencyTaskRuntimeAbiCompletionJson(
             semantic_lowering_plan
                 .concurrency_task_runtime_interop_cancellation_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_concurrency_replay_race_guard_lowering_replay_key)
      << ",\"objc_concurrency_async_function_await_and_continuation_lowering\":"
      << BuildConcurrencyAsyncDirectCallLoweringJson(
             pipeline_result.concurrency_async_source_closure_summary,
             semantic_lowering_plan
                 .concurrency_async_continuation_lowering_contract,
             semantic_lowering_plan
                 .concurrency_await_lowering_suspension_state_lowering_contract,
             semantic_lowering_plan
                 .concurrency_async_continuation_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_await_lowering_suspension_state_lowering_replay_key)
      << ",\"objc_concurrency_suspension_autorelease_and_cleanup_integration\":"
      << BuildConcurrencySuspensionCleanupIntegrationJson(
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_contract,
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_replay_key,
             ownership_aware_lowering_plan
                 .autoreleasepool_scope_lowering_contract,
             ownership_aware_lowering_plan
                 .autoreleasepool_scope_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_async_continuation_lowering_contract,
             semantic_lowering_plan
                 .concurrency_await_lowering_suspension_state_lowering_contract,
             semantic_lowering_plan
                 .concurrency_async_continuation_lowering_replay_key,
             semantic_lowering_plan
                 .concurrency_await_lowering_suspension_state_lowering_replay_key);
}

}  // namespace objc3::artifacts::frontend
