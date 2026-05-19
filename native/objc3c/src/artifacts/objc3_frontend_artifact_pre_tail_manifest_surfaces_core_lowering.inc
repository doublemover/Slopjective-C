namespace {

void WriteObjc3FrontendArtifactPreTailCoreRuntimeSurfaces(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  WriteObjc3FrontendRuntimeBootstrapManifestOrchestration(
      manifest, pipeline_result, conformance_report_plan, runtime_metadata_plan,
      runtime_registration_plan);
  manifest
      << ",\"objc_id_class_sel_object_pointer_typecheck_surface\":"
         "{\"id_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .id_typecheck_sites
      << ",\"class_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .class_typecheck_sites
      << ",\"sel_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .sel_typecheck_sites
      << ",\"object_pointer_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .object_pointer_typecheck_sites
      << ",\"total_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .total_typecheck_sites
      << ",\"replay_key\":\""
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key
      << "\",\"deterministic_handoff\":"
      << (core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
                  .deterministic
              ? "true"
              : "false")
      << "}";
  WriteDispatchRuntimeAbiManifestSurfaces(
      manifest, core_lowering_plan.dispatch_surface_classification_snapshot,
      core_lowering_plan.dispatch_surface_classification_replay_key,
      core_lowering_plan.message_send_selector_lowering_snapshot,
      core_lowering_plan.message_send_selector_lowering_replay_key,
      core_lowering_plan.dispatch_abi_marshalling_snapshot,
      core_lowering_plan.dispatch_abi_marshalling_replay_key,
      core_lowering_plan.nil_receiver_semantics_foldability_snapshot,
      core_lowering_plan.nil_receiver_semantics_foldability_replay_key,
      core_lowering_plan.super_dispatch_method_family_snapshot,
      core_lowering_plan.super_dispatch_method_family_replay_key,
      core_lowering_plan.runtime_link_host_link_snapshot,
      core_lowering_plan.runtime_link_host_link_replay_key,
      core_lowering_plan.runtime_dispatch_lowering_abi_snapshot,
      core_lowering_plan.runtime_dispatch_lowering_abi_replay_key);
}

void WriteObjc3FrontendArtifactPreTailLanguageLoweringSurfaces(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary) {
  WriteOwnershipReleaseManifestSurfaces(
      manifest,
      ownership_aware_lowering_plan.ownership_qualifier_lowering_contract,
      ownership_aware_lowering_plan.ownership_qualifier_lowering_replay_key,
      ownership_aware_lowering_plan.retain_release_operation_lowering_contract,
      ownership_aware_lowering_plan
          .retain_release_operation_lowering_replay_key,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key,
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_contract,
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_replay_key,
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_contract,
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_replay_key);
  WriteBlockManifestSurfaces(
      manifest, block_lowering_plan.block_literal_capture_lowering_contract,
      block_lowering_plan.block_literal_capture_lowering_replay_key,
      block_lowering_plan.block_source_model_completion_contract,
      block_lowering_plan.block_source_model_completion_replay_key,
      block_lowering_plan.block_source_storage_annotation_contract,
      block_lowering_plan.block_source_storage_annotation_replay_key,
      block_lowering_plan.block_abi_invoke_trampoline_lowering_contract,
      block_lowering_plan.block_abi_invoke_trampoline_lowering_replay_key,
      block_lowering_plan.block_storage_escape_lowering_contract,
      block_lowering_plan.block_storage_escape_lowering_replay_key,
      block_lowering_plan.block_copy_dispose_lowering_contract,
      block_lowering_plan.block_copy_dispose_lowering_replay_key,
      block_lowering_plan.block_determinism_perf_baseline_lowering_contract,
      block_lowering_plan
          .block_determinism_perf_baseline_lowering_replay_key);
  WriteTypeSystemManifestSurfaces(
      manifest,
      type_system_lowering_plan.lightweight_generic_constraint_lowering_contract,
      type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key,
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_contract,
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key,
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_contract,
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key,
      type_system_lowering_plan.variance_bridge_cast_lowering_contract,
      type_system_lowering_plan.variance_bridge_cast_lowering_replay_key,
      type_system_lowering_plan.generic_metadata_abi_lowering_contract,
      type_system_lowering_plan.generic_metadata_abi_lowering_replay_key);
  manifest
      << ",\"objc_type_system_optional_keypath_lowering_contract\":"
      << BuildTypeSystemOptionalKeypathLoweringContractJson(
             core_lowering_plan.type_system_optional_keypath_lowering_contract,
             type_system_type_semantic_model_summary,
             type_system_type_semantic_model_summary.replay_key,
             core_lowering_plan.message_send_selector_lowering_replay_key,
             core_lowering_plan.dispatch_abi_marshalling_replay_key,
             core_lowering_plan.nil_receiver_semantics_foldability_replay_key,
             core_lowering_plan.type_system_optional_keypath_lowering_replay_key)
      << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
      << BuildControlFlowControlFlowSafetyLoweringContractJson(
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_contract,
             pipeline_result.control_flow_control_flow_semantic_model_summary,
             pipeline_result.control_flow_control_flow_semantic_model_summary
                 .replay_key,
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_replay_key)
      << ",\"objc_type_system_optional_keypath_runtime_helper_contract\":"
      << BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
             core_lowering_plan.type_system_optional_keypath_lowering_contract,
             runtime_registration_plan.runtime_support_library_link_wiring,
             core_lowering_plan.type_system_optional_keypath_lowering_replay_key);
}

}  // namespace
