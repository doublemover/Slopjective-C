#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"

#include "artifacts/objc3_frontend_artifact_lowering_contracts.h"
#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

namespace {

void RecordSemanticLoweringPlanFailure(
    objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure
        &failure,
    const char *message) {
  objc3::artifacts::frontend::RecordObjc3FrontendArtifactPostPipelineFailure(
      failure, "O3L300", message);
}

}  // namespace

Objc3FrontendArtifactSemanticLoweringPlan
BuildObjc3FrontendArtifactSemanticLoweringPlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactSemanticLoweringPlan plan;

  plan.dispatch_dispatch_control_lowering_contract =
      objc3::artifacts::frontend::BuildDispatchDispatchControlLoweringContract(
          pipeline_result.dispatch_dispatch_intent_semantic_model_summary,
          pipeline_result.dispatch_dispatch_intent_legality_summary,
          pipeline_result.dispatch_dispatch_intent_compatibility_summary);
  if (!IsValidObjc3DispatchDispatchControlLoweringContract(
          plan.dispatch_dispatch_control_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid dispatch-control lowering contract");
  }
  plan.dispatch_dispatch_control_lowering_replay_key =
      Objc3DispatchDispatchControlLoweringReplayKey(
          plan.dispatch_dispatch_control_lowering_contract);

  plan.metaprogramming_expansion_lowering_contract =
      objc3::artifacts::frontend::BuildMetaprogrammingExpansionLoweringContract(
          pipeline_result
              .metaprogramming_property_behavior_source_completion_summary,
          pipeline_result.metaprogramming_derive_expansion_inventory_summary,
          pipeline_result
              .metaprogramming_macro_safety_sandbox_determinism_summary,
          pipeline_result
              .metaprogramming_property_behavior_legality_compatibility_summary);
  if (!IsValidObjc3MetaprogrammingExpansionLoweringContract(
          plan.metaprogramming_expansion_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid Part 10 expansion lowering contract");
  }
  plan.metaprogramming_expansion_lowering_replay_key =
      Objc3MetaprogrammingExpansionLoweringReplayKey(
          plan.metaprogramming_expansion_lowering_contract);
  plan.metaprogramming_derived_method_bundles =
      objc3::artifacts::frontend::BuildMetaprogrammingDerivedMethodBundles(
          program);
  plan.metaprogramming_macro_artifact_bundles =
      objc3::artifacts::frontend::BuildMetaprogrammingMacroArtifactBundles(
          program);
  plan.metaprogramming_property_behavior_artifact_bundles =
      objc3::artifacts::frontend::
          BuildMetaprogrammingPropertyBehaviorArtifactBundles(program);
  plan.metaprogramming_synthesized_artifact_emission_contract =
      objc3::artifacts::frontend::
          BuildMetaprogrammingSynthesizedArtifactEmissionContract(
              plan.metaprogramming_expansion_lowering_contract,
              plan.metaprogramming_derived_method_bundles,
              plan.metaprogramming_macro_artifact_bundles,
              plan.metaprogramming_property_behavior_artifact_bundles);
  if (!IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
          plan.metaprogramming_synthesized_artifact_emission_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid Part 10 synthesized artifact emission contract");
  }
  plan.metaprogramming_synthesized_artifact_emission_replay_key =
      Objc3MetaprogrammingSynthesizedArtifactEmissionReplayKey(
          plan.metaprogramming_synthesized_artifact_emission_contract);

  plan.ownership_system_extension_lowering_contract =
      objc3::artifacts::frontend::
          BuildOwnershipSystemExtensionLoweringContract(
              pipeline_result.ownership_system_extension_semantic_model_summary,
              pipeline_result
                  .ownership_resource_move_use_after_move_semantics_summary,
              pipeline_result.ownership_borrowed_pointer_escape_analysis_summary,
              pipeline_result
                  .ownership_capture_list_retainable_family_legality_completion_summary);
  if (!IsValidObjc3OwnershipSystemExtensionLoweringContract(
          plan.ownership_system_extension_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid system-extension lowering contract");
  }
  plan.ownership_system_extension_lowering_replay_key =
      Objc3OwnershipSystemExtensionLoweringReplayKey(
          plan.ownership_system_extension_lowering_contract);
  plan.ownership_borrowed_retainable_abi_completion_replay_key =
      objc3::artifacts::frontend::
          BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
              plan.ownership_system_extension_lowering_contract,
              pipeline_result.ownership_system_extension_source_closure_summary,
              pipeline_result.ownership_retainable_c_family_source_completion_summary);

  plan.concurrency_async_continuation_lowering_contract =
      objc3::artifacts::frontend::
          BuildConcurrencyAsyncContinuationLoweringContract(
              pipeline_result
                  .concurrency_async_effect_suspension_semantic_model_summary,
              pipeline_result.concurrency_async_diagnostics_compatibility_summary);
  plan.concurrency_await_lowering_suspension_state_lowering_contract =
      objc3::artifacts::frontend::
          BuildConcurrencyAwaitLoweringSuspensionStateLoweringContract(
              pipeline_result.concurrency_await_suspension_resume_semantic_summary);
  if (!IsValidObjc3AsyncContinuationLoweringContract(
          plan.concurrency_async_continuation_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid async continuation lowering contract");
  }
  if (!IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
          plan.concurrency_await_lowering_suspension_state_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid await suspension lowering contract");
  }
  plan.concurrency_async_continuation_lowering_replay_key =
      Objc3AsyncContinuationLoweringReplayKey(
          plan.concurrency_async_continuation_lowering_contract);
  plan.concurrency_await_lowering_suspension_state_lowering_replay_key =
      Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
          plan.concurrency_await_lowering_suspension_state_lowering_contract);
  plan.concurrency_actor_isolation_sendability_lowering_contract =
      objc3::artifacts::frontend::
          BuildConcurrencyActorIsolationSendabilityLoweringContract(
              pipeline_result.concurrency_executor_hop_affinity_compatibility_summary);
  if (!IsValidObjc3ActorIsolationSendabilityLoweringContract(
          plan.concurrency_actor_isolation_sendability_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid actor isolation sendability lowering contract");
  }
  plan.concurrency_actor_isolation_sendability_lowering_replay_key =
      Objc3ActorIsolationSendabilityLoweringReplayKey(
          plan.concurrency_actor_isolation_sendability_lowering_contract);
  plan.concurrency_actor_lowering_metadata_contract =
      objc3::artifacts::frontend::BuildConcurrencyActorLoweringMetadataContract(
          pipeline_result.concurrency_actor_member_isolation_source_closure_summary,
          pipeline_result
              .concurrency_actor_isolation_sendability_enforcement_summary,
          pipeline_result.concurrency_actor_race_hazard_escape_diagnostics_summary);
  if (!IsValidObjc3ActorLoweringMetadataContract(
          plan.concurrency_actor_lowering_metadata_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid actor lowering metadata contract");
  }
  plan.concurrency_actor_lowering_metadata_replay_key =
      Objc3ActorLoweringMetadataReplayKey(
          plan.concurrency_actor_lowering_metadata_contract);
  plan.concurrency_task_runtime_interop_cancellation_lowering_contract =
      objc3::artifacts::frontend::
          BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract(
              pipeline_result
                  .concurrency_task_executor_cancellation_semantic_model_summary,
              pipeline_result
                  .concurrency_structured_task_cancellation_semantic_summary,
              pipeline_result
                  .concurrency_executor_hop_affinity_compatibility_summary);
  if (!IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
          plan.concurrency_task_runtime_interop_cancellation_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid task runtime interop cancellation lowering contract");
  }
  plan.concurrency_task_runtime_interop_cancellation_lowering_replay_key =
      Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
          plan.concurrency_task_runtime_interop_cancellation_lowering_contract);
  plan.concurrency_concurrency_replay_race_guard_lowering_contract =
      objc3::artifacts::frontend::
          BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract(
              pipeline_result
                  .concurrency_task_executor_cancellation_semantic_model_summary,
              pipeline_result
                  .concurrency_structured_task_cancellation_semantic_summary,
              pipeline_result
                  .concurrency_executor_hop_affinity_compatibility_summary,
              plan.concurrency_actor_isolation_sendability_lowering_contract);
  if (!IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
          plan.concurrency_concurrency_replay_race_guard_lowering_contract)) {
    RecordSemanticLoweringPlanFailure(
        plan.post_pipeline_failure,
        "LLVM IR emission failed: invalid concurrency replay race guard lowering contract");
  }
  plan.concurrency_concurrency_replay_race_guard_lowering_replay_key =
      Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
          plan.concurrency_concurrency_replay_race_guard_lowering_contract);

  return plan;
}
