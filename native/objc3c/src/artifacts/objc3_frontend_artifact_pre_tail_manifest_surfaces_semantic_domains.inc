namespace {

void WriteObjc3FrontendArtifactPreTailInteropAndMetaprogrammingSurfaces(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan,
    const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan) {
  WriteInteropManifestSurfaces(
      manifest, pipeline_result.interop_interop_semantic_model_summary,
      pipeline_result.interop_interop_runtime_parity_summary,
      pipeline_result.interop_cpp_interop_interaction_summary,
      pipeline_result.interop_swift_interop_isolation_summary,
      interop_lowering_plan.interop_foreign_surface_interface_preservation_summary,
      interop_lowering_plan.interop_header_module_bridge_generation_summary,
      interop_lowering_plan.interop_interop_lowering_contract,
      interop_lowering_plan.interop_interop_lowering_replay_key,
      interop_lowering_plan.interop_foreign_call_lifetime_lowering_contract,
      interop_lowering_plan.interop_foreign_call_lifetime_lowering_replay_key,
      interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_contract,
      interop_lowering_plan.interop_ffi_metadata_interface_preservation_replay_key);
  WriteMetaprogrammingManifestSurfaces(
      manifest,
      pipeline_result
          .metaprogramming_expansion_behavior_semantic_model_summary,
      pipeline_result.metaprogramming_derive_expansion_inventory_summary,
      pipeline_result
          .metaprogramming_macro_safety_sandbox_determinism_summary,
      pipeline_result
          .metaprogramming_property_behavior_legality_compatibility_summary,
      pipeline_result
          .metaprogramming_property_behavior_source_completion_summary,
      semantic_lowering_plan.metaprogramming_expansion_lowering_contract,
      semantic_lowering_plan.metaprogramming_expansion_lowering_replay_key,
      semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_contract,
      semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_replay_key,
      artifact_preservation_plan
          .metaprogramming_module_interface_replay_preservation_summary);
}

void WriteObjc3FrontendArtifactPreTailDispatchConcurrencyOwnershipSurfaces(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan) {
  WriteDispatchManifestSurfaces(
      manifest, pipeline_result.dispatch_dispatch_intent_semantic_model_summary,
      pipeline_result.dispatch_dispatch_intent_legality_summary,
      pipeline_result.dispatch_dispatch_intent_compatibility_summary,
      semantic_lowering_plan.dispatch_dispatch_control_lowering_contract,
      semantic_lowering_plan.dispatch_dispatch_control_lowering_replay_key,
      artifact_preservation_plan
          .dispatch_dispatch_metadata_interface_preservation_summary);
  WriteConcurrencyManifestSurfaces(
      manifest,
      pipeline_result.concurrency_actor_member_isolation_source_closure_summary,
      pipeline_result.concurrency_actor_isolation_sendable_semantic_model_summary,
      pipeline_result
          .concurrency_actor_isolation_sendability_enforcement_summary,
      pipeline_result.concurrency_actor_race_hazard_escape_diagnostics_summary,
      semantic_lowering_plan.concurrency_actor_lowering_metadata_contract,
      semantic_lowering_plan.concurrency_actor_lowering_metadata_replay_key,
      pipeline_result.concurrency_task_group_cancellation_source_closure_summary,
      pipeline_result.concurrency_async_effect_suspension_semantic_model_summary,
      pipeline_result.concurrency_task_executor_cancellation_semantic_model_summary);
  WriteOwnershipManifestSurfaces(
      manifest,
      pipeline_result.ownership_system_extension_semantic_model_summary,
      pipeline_result.effects_ownership_semantic_model_summary,
      pipeline_result.cross_module_semantic_contracts_diagnostics_summary,
      pipeline_result.ownership_resource_move_use_after_move_semantics_summary,
      pipeline_result.ownership_borrowed_pointer_escape_analysis_summary,
      pipeline_result
          .ownership_capture_list_retainable_family_legality_completion_summary,
      semantic_lowering_plan.ownership_system_extension_lowering_contract,
      pipeline_result.ownership_system_extension_source_closure_summary,
      pipeline_result
          .ownership_retainable_c_family_source_completion_summary,
      semantic_lowering_plan.ownership_system_extension_lowering_replay_key,
      semantic_lowering_plan
          .ownership_borrowed_retainable_abi_completion_replay_key);
}

void WriteObjc3FrontendArtifactPreTailRuntimeAndSummarySurfaces(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactFunctionManifest &function_manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary) {
  WriteConcurrencyRuntimeManifestSurfaces(
      manifest,
      pipeline_result.concurrency_structured_task_cancellation_semantic_summary,
      pipeline_result.concurrency_executor_hop_affinity_compatibility_summary,
      pipeline_result.concurrency_await_suspension_resume_semantic_summary,
      pipeline_result.concurrency_async_diagnostics_compatibility_summary,
      semantic_lowering_plan.concurrency_async_continuation_lowering_contract,
      semantic_lowering_plan
          .concurrency_await_lowering_suspension_state_lowering_contract,
      semantic_lowering_plan.concurrency_async_continuation_lowering_replay_key,
      semantic_lowering_plan
          .concurrency_await_lowering_suspension_state_lowering_replay_key,
      pipeline_result.concurrency_task_executor_cancellation_semantic_model_summary,
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
          .concurrency_concurrency_replay_race_guard_lowering_replay_key,
      pipeline_result.concurrency_async_source_closure_summary,
      core_lowering_plan.control_flow_control_flow_safety_lowering_contract,
      core_lowering_plan.control_flow_control_flow_safety_lowering_replay_key,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key);
  WriteSemanticSummaryManifestSurfaces(
      manifest, pipeline_result.error_handling_error_semantic_model_summary,
      pipeline_result.error_handling_try_do_catch_semantic_summary,
      pipeline_result.error_handling_error_bridge_legality_summary,
      pipeline_result.control_flow_control_flow_semantic_model_summary,
      type_system_type_semantic_model_summary,
      pipeline_result.symbol_graph_scope_resolution_summary,
      function_manifest);
}

}  // namespace
