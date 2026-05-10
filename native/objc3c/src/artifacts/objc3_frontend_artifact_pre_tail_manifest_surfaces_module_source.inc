namespace {

void WriteObjc3FrontendArtifactPreTailModuleAndErrorSurfaces(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan) {
  WriteCrossModuleManifestSurfaces(
      manifest, program, pipeline_result.parser_contract_snapshot,
      runtime_import_plan.module_import_graph_lowering_contract,
      runtime_import_plan.module_import_graph_lowering_replay_key,
      runtime_import_plan.runtime_aware_import_module_frontend_closure,
      runtime_import_plan
          .cross_module_runtime_metadata_semantic_preservation,
      runtime_import_plan.imported_runtime_metadata_semantic_rules,
      runtime_import_plan.serialized_runtime_metadata_import_lowering,
      runtime_import_plan.serialized_runtime_metadata_artifact_reuse,
      runtime_import_plan.cross_module_build_runtime_orchestration,
      module_lowering_plan.namespace_collision_shadowing_lowering_contract,
      module_lowering_plan.namespace_collision_shadowing_lowering_replay_key,
      module_lowering_plan.public_private_api_partition_lowering_contract,
      module_lowering_plan.public_private_api_partition_lowering_replay_key,
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_contract,
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_replay_key,
      module_lowering_plan.cross_module_conformance_lowering_contract,
      module_lowering_plan.cross_module_conformance_lowering_replay_key);
  WriteErrorHandlingManifestSurfaces(
      manifest, error_lowering_plan.throws_propagation_lowering_contract,
      error_lowering_plan.throws_propagation_lowering_replay_key,
      error_lowering_plan.result_like_lowering_contract,
      error_lowering_plan.result_like_lowering_replay_key,
      error_lowering_plan.ns_error_bridging_lowering_contract,
      error_lowering_plan.ns_error_bridging_lowering_replay_key,
      error_lowering_plan.unwind_cleanup_lowering_contract,
      error_lowering_plan.unwind_cleanup_lowering_replay_key,
      error_lowering_plan
          .deterministic_error_handling_throws_abi_propagation_lowering,
      interop_lowering_plan
          .error_handling_result_and_bridging_artifact_replay_summary);
}

void WriteObjc3FrontendArtifactPreTailSourceAndToolingSurfaces(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan) {
  WriteSourceClosureManifestSurfaces(
      manifest, pipeline_result.object_pointer_nullability_generics_summary,
      pipeline_result.type_system_type_source_closure_summary,
      pipeline_result.control_flow_control_flow_source_closure_summary,
      pipeline_result.error_handling_error_source_closure_summary,
      pipeline_result.concurrency_async_source_closure_summary,
      pipeline_result.ownership_system_extension_source_closure_summary,
      pipeline_result
          .ownership_cleanup_resource_capture_source_completion_summary,
      pipeline_result
          .ownership_retainable_c_family_source_completion_summary,
      pipeline_result.dispatch_dispatch_intent_source_closure_summary,
      pipeline_result.dispatch_dispatch_intent_source_completion_summary,
      pipeline_result
          .metaprogramming_metaprogramming_source_closure_summary,
      pipeline_result
          .metaprogramming_macro_package_provenance_source_completion_summary,
      pipeline_result
          .metaprogramming_property_behavior_source_completion_summary,
      pipeline_result.interop_foreign_import_source_closure_summary,
      pipeline_result
          .interop_cpp_swift_interop_annotation_source_completion_summary);
  WriteToolingManifestSurfaces(
      manifest,
      pipeline_result.tooling_diagnostics_migrator_source_inventory_summary,
      pipeline_result.tooling_migration_canonicalization_source_completion_summary,
      pipeline_result.tooling_diagnostic_taxonomy_portability_contract_summary,
      pipeline_result.tooling_feature_specific_fixit_synthesis_summary,
      conformance_report_plan.tooling_legacy_canonical_migration_semantics_summary,
      conformance_report_plan
          .tooling_machine_readable_conformance_report_contract_summary,
      conformance_report_plan
          .tooling_feature_aware_conformance_report_emission_summary,
      conformance_report_plan
          .tooling_corpus_sharding_release_evidence_packaging_summary);
}

}  // namespace
