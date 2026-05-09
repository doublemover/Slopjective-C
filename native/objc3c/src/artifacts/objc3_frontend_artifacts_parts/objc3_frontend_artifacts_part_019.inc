           << "\""
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"runtime_final_release_evidence_descaffolding_implementation_surface\":{\"contract_id\":\""
           << kObjc3RuntimeFinalReleaseEvidenceDescaffoldingImplementationSurfaceContractId
           << "\",\"runtime_release_candidate_claim_abi_surface_contract_id\":\""
           << kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"private_release_candidate_evidence_testing_boundary\":[\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol << "\"]"
           << ",\"release_candidate_evidence_snapshot_symbol\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol
           << "\",\"release_candidate_evidence_snapshot_type\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceSnapshotType
           << "\",\"validation_artifact_name\":\"module.objc3-conformance-validation.json\""
           << ",\"release_evidence_operation_artifact_name\":\"module.objc3-release-evidence-operation.json\""
           << ",\"dashboard_status_artifact_name\":\"module.objc3-dashboard-status.json\""
           << ",\"advanced_feature_gate_artifact_name\":\"module.objc3-advanced-feature-gate.json\""
           << ",\"release_candidate_matrix_artifact_name\":\"module.objc3-release-candidate-matrix.json\""
           << ",\"authoritative_probe_path\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceProbePath
           << "\",\"implementation_model\":\""
           << kObjc3RuntimeReleaseCandidateEvidenceImplementationModel
           << "\""
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
  manifest << "  \"lowering_id_class_sel_object_pointer_typecheck\":{\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"lane_contract\":\"" << kObjc3IdClassSelObjectPointerTypecheckLaneContract
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_surface_classification\":{\"replay_key\":\""
           << dispatch_surface_classification_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3DispatchSurfaceClassificationContractId
           << "\",\"deterministic_handoff\":"
           << (dispatch_surface_classification_contract.deterministic ? "true"
                                                                     : "false")
           << "},\n";
  manifest << "  \"lowering_message_send_selector_lowering\":{\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3MessageSendSelectorLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_abi_marshalling\":{\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"lane_contract\":\"" << kObjc3DispatchAbiMarshallingLaneContract
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nil_receiver_semantics_foldability\":{\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NilReceiverSemanticsFoldabilityLaneContract
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_control_flow_control_flow_safety\":{\"replay_key\":\""
           << control_flow_control_flow_safety_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ControlFlowControlFlowSafetyLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (control_flow_control_flow_safety_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_super_dispatch_method_family\":{\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"lane_contract\":\"" << kObjc3SuperDispatchMethodFamilyLaneContract
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_runtime_link_host_link\":{\"replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RuntimeLinkHostLinkLaneContract
           << "\",\"deterministic_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"lowering_ownership_qualifier\":{\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3OwnershipQualifierLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_retain_release_operation\":{\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RetainReleaseOperationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_autoreleasepool_scope\":{\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_weak_unowned_semantics\":{\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3WeakUnownedSemanticsLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_arc_diagnostics_fixit\":{\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_literal_capture\":{\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockLiteralCaptureLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_abi_invoke_trampoline\":{\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_storage_escape\":{\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockStorageEscapeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_copy_dispose\":{\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockCopyDisposeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_determinism_perf_baseline\":{\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockDeterminismPerfBaselineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_lightweight_generic_constraint\":{\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3LightweightGenericsConstraintLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nullability_flow_warning_precision\":{\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_protocol_qualified_object_type\":{\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_variance_bridge_cast\":{\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3VarianceBridgeCastLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_generic_metadata_abi\":{\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3GenericMetadataAbiLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_module_import_graph\":{\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ModuleImportGraphLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_namespace_collision_shadowing\":{\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3NamespaceCollisionShadowingLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_public_private_api_partition\":{\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3PublicPrivateApiPartitionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_incremental_module_cache_invalidation\":{\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_cross_module_conformance\":{\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3CrossModuleConformanceLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_throws_propagation\":{\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ThrowsPropagationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_result_like\":{\"replay_key\":\""
           << result_like_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ResultLikeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (result_like_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_ns_error_bridging\":{\"replay_key\":\""
           << ns_error_bridging_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3NSErrorBridgingLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (ns_error_bridging_lowering_contract.deterministic ? "true"
                                                                 : "false")
           << "},\n";
  manifest << "  \"lowering_unwind_cleanup\":{\"replay_key\":\""
           << unwind_cleanup_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3UnwindCleanupLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (unwind_cleanup_lowering_contract.deterministic ? "true"
                                                              : "false")
           << "},\n";
  manifest
      << "  \"lowering_error_handling_throws_abi_propagation\":{\"replay_key\":\""
      << error_handling_throws_abi_propagation_lowering_replay_key
      << "\",\"contract_id\":\""
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
      << "\",\"deterministic_handoff\":"
      << (deterministic_error_handling_throws_abi_propagation_lowering ? "true"
                                                              : "false")
      << "},\n";
  manifest
      << "  \"lowering_error_handling_result_and_bridging_artifact_replay\":{\"replay_key\":\""
      << error_handling_result_and_bridging_artifact_replay_summary.replay_key
      << "\",\"contract_id\":\""
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId
      << "\",\"deterministic_handoff\":"
      << (error_handling_result_and_bridging_artifact_replay_summary.deterministic
              ? "true"
              : "false")
      << "},\n";
  manifest << "  \"semantic_canonical_type_metadata\":";
  objc3::artifacts::json::WriteSemanticTypeMetadataHandoffManifestObject(
      manifest, type_metadata_handoff);
  manifest << ",\n";
  manifest << "  \"globals\": ";
  objc3::artifacts::json::WriteProgramGlobalsManifestArray(
      manifest, program.globals, resolved_global_values);
  manifest << ",\n";
  manifest << "  \"functions\": ";
  objc3::artifacts::json::WriteFunctionDeclarationsManifestArray(
      manifest, manifest_functions);
  manifest << ",\n";
  manifest << "  \"interfaces\": ";
  objc3::artifacts::json::WriteRuntimeMetadataInterfaceManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"implementations\": ";
  objc3::artifacts::json::WriteRuntimeMetadataImplementationManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"protocols\": ";
  objc3::artifacts::json::WriteRuntimeMetadataProtocolManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"categories\": ";
  objc3::artifacts::json::WriteRuntimeMetadataCategoryManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"runtime_metadata_source_records\": ";
  objc3::artifacts::json::WriteRuntimeMetadataSourceRecordSetManifestObject(
      manifest, runtime_metadata_source_records);
  manifest << "\n";
  manifest << "}\n";
  bundle.manifest_json = manifest.str();
  bundle.runtime_metadata_binary = executable_metadata_runtime_ingest_binary_payload;
  if (IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          runtime_aware_import_module_frontend_closure)) {
    bundle.runtime_aware_import_module_artifact_json =
        BuildRuntimeAwareImportModuleArtifactJson(
            runtime_aware_import_module_frontend_closure,
            runtime_metadata_source_records,
            BuildTypeSystemOptionalKeypathLoweringContractJson(
                type_system_optional_keypath_lowering_contract,
                type_system_type_semantic_model_summary,
                type_system_type_semantic_model_summary.replay_key,
                message_send_selector_lowering_replay_key,
                dispatch_abi_marshalling_replay_key,
                nil_receiver_semantics_foldability_replay_key,
                type_system_optional_keypath_lowering_replay_key),
            BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
                type_system_optional_keypath_lowering_contract,
                runtime_support_library_link_wiring,
                type_system_optional_keypath_lowering_replay_key),
            BuildTypeSystemGenericContractPreservationJson(
                type_metadata_handoff, type_system_type_semantic_model_summary),
            BuildTypeSystemNullabilityContractPreservationJson(
                type_system_type_semantic_model_summary),
            BuildTypeSystemProtocolContractPreservationJson(
                program, runtime_metadata_source_records,
                type_system_type_semantic_model_summary),
            BuildErrorHandlingResultAndBridgingArtifactReplaySummaryJson(
                error_handling_result_and_bridging_artifact_replay_summary),
            BuildConcurrencyActorMailboxRuntimeImportSummaryJson(
                BuildConcurrencyActorMailboxRuntimeImportSummary(
                    concurrency_actor_lowering_metadata_contract,
                    concurrency_actor_lowering_metadata_replay_key,
                    concurrency_actor_isolation_sendability_lowering_replay_key)),
            BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
                interop_foreign_surface_interface_preservation_summary),
            BuildInteropHeaderModuleBridgeGenerationSummaryJson(
                interop_header_module_bridge_generation_summary),
            BuildInteropFfiMetadataInterfacePreservationContractJson(
                interop_foreign_call_lifetime_lowering_contract,
                interop_foreign_call_lifetime_lowering_replay_key,
                interop_foreign_surface_interface_preservation_summary,
                interop_ffi_metadata_interface_preservation_contract,
                interop_ffi_metadata_interface_preservation_replay_key),
            BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
                metaprogramming_module_interface_replay_preservation_summary),
            BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
                metaprogramming_macro_host_process_cache_runtime_integration_summary),
            BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
                dispatch_dispatch_metadata_interface_preservation_summary),
            BuildRuntimeBlockOwnershipArtifactPreservationSummaryJson(
                runtime_block_ownership_artifact_preservation_summary),
            BuildRuntimeStorageReflectionArtifactPreservationSummaryJson(
                runtime_storage_reflection_artifact_preservation_summary),
            serialized_runtime_metadata_artifact_reuse,
            serialized_runtime_metadata_reuse_records);
  }
  if (interop_header_module_bridge_generation_summary.runtime_generation_ready &&
      interop_header_module_bridge_generation_summary.deterministic) {
    bundle.interop_bridge_header_artifact_text = BuildInteropBridgeHeaderArtifactText(
        program, runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
    bundle.interop_bridge_module_artifact_text = BuildInteropBridgeModuleArtifactText(
        runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
    bundle.interop_bridge_artifact_json = BuildInteropBridgeArtifactJson(
        program, runtime_aware_import_module_frontend_closure,
        interop_header_module_bridge_generation_summary);
  }
  bundle.metaprogramming_macro_host_process_cache_runtime_integration_ready =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .runtime_import_artifact_ready;
  bundle.metaprogramming_macro_host_process_cache_runtime_integration_replay_key =
      metaprogramming_macro_host_process_cache_runtime_integration_summary.replay_key;
  bundle
      .metaprogramming_macro_host_process_cache_runtime_integration_cache_root_relative_path =
      metaprogramming_macro_host_process_cache_runtime_integration_summary
          .cache_root_relative_path;
  if (error_handling_result_and_bridging_artifact_replay_summary
          .binary_artifact_replay_ready) {
    bundle.error_handling_result_bridge_artifact_replay_json =
        BuildErrorHandlingResultAndBridgingArtifactReplaySummaryJson(
            error_handling_result_and_bridging_artifact_replay_summary);
  }
  if (IsReadyObjc3VersionedConformanceReportLoweringSummary(
          versioned_conformance_report_lowering)) {
    bundle.versioned_conformance_report_artifact_json =
        BuildVersionedConformanceReportArtifactJson(
            versioned_conformance_report_lowering, options, pipeline_result,
            frontend_compatibility_strictness_claim_semantics,
            tooling_feature_aware_conformance_report_emission_summary,
            tooling_corpus_sharding_release_evidence_packaging_summary);
  }
  bundle.runtime_aware_import_module_frontend_closure_summary =
      runtime_aware_import_module_frontend_closure;
  bundle.versioned_conformance_report_lowering_summary =
      versioned_conformance_report_lowering;
  bundle.runtime_registration_descriptor_image_root_source_surface_summary =
      runtime_registration_descriptor_image_root_source_surface;
  bundle.runtime_registration_descriptor_frontend_closure_summary =
      runtime_registration_descriptor_frontend_closure;
  bundle.runtime_block_ownership_artifact_preservation_summary =
      runtime_block_ownership_artifact_preservation_summary;
  bundle.runtime_storage_reflection_artifact_preservation_summary =
      runtime_storage_reflection_artifact_preservation_summary;
  bundle.runtime_translation_unit_registration_manifest_summary =
      runtime_translation_unit_registration_manifest;
  bundle.runtime_bootstrap_legality_semantics_summary =
      runtime_bootstrap_legality_semantics;
  bundle.runtime_bootstrap_legality_failure_contract_summary =
      runtime_bootstrap_legality_failure_contract;
  bundle.runtime_bootstrap_failure_restart_semantics_summary =
      runtime_bootstrap_failure_restart_semantics;
  bundle.frontend_compatibility_strictness_claim_semantics_summary =
      frontend_compatibility_strictness_claim_semantics;
  bundle.tooling_legacy_canonical_migration_semantics_summary =
      tooling_legacy_canonical_migration_semantics_summary;
  bundle.tooling_machine_readable_conformance_report_contract_summary =
      tooling_machine_readable_conformance_report_contract_summary;
  bundle.tooling_feature_aware_conformance_report_emission_summary =
      tooling_feature_aware_conformance_report_emission_summary;
  bundle.tooling_corpus_sharding_release_evidence_packaging_summary =
      tooling_corpus_sharding_release_evidence_packaging_summary;
  bundle.runtime_bootstrap_api_summary = runtime_bootstrap_api;
  bundle.runtime_bootstrap_semantics_summary = runtime_bootstrap_semantics;
  bundle.runtime_bootstrap_lowering_summary = runtime_bootstrap_lowering;

  if (!post_pipeline_failure_code.empty()) {
    if (!options.emit_ir && !options.emit_object) {
      return bundle;
    }
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, post_pipeline_failure_code, post_pipeline_failure_message)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }

  if (!options.emit_ir && !options.emit_object) {
    return bundle;
  }

  Objc3IRFrontendMetadata ir_frontend_metadata;
  ir_frontend_metadata.language_version = options.language_version;
  ir_frontend_metadata.language_profile = LanguageProfileName(options.language_profile);
  ir_frontend_metadata.arc_mode = ArcModeName(options.arc_mode);
  ir_frontend_metadata.arc_mode_enabled =
      options.arc_mode == Objc3FrontendArcMode::kEnabled;
  ir_frontend_metadata.versioned_conformance_report_lowering_ready =
      IsReadyObjc3VersionedConformanceReportLoweringSummary(
          versioned_conformance_report_lowering);
  ir_frontend_metadata.versioned_conformance_report_lowering_replay_key =
      versioned_conformance_report_lowering.replay_key;
  ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;
  ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;
  ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;
  ir_frontend_metadata.declared_interfaces = interface_implementation_summary.declared_interfaces;
  ir_frontend_metadata.declared_implementations = interface_implementation_summary.declared_implementations;
  ir_frontend_metadata.resolved_interface_symbols = interface_implementation_summary.resolved_interfaces;
  ir_frontend_metadata.resolved_implementation_symbols = interface_implementation_summary.resolved_implementations;
  ir_frontend_metadata.interface_method_symbols = interface_implementation_summary.interface_method_symbols;
  ir_frontend_metadata.implementation_method_symbols = interface_implementation_summary.implementation_method_symbols;
  ir_frontend_metadata.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;
  ir_frontend_metadata.declared_protocols = protocol_category_summary.declared_protocols;
  ir_frontend_metadata.declared_categories = protocol_category_summary.declared_categories;
  ir_frontend_metadata.resolved_protocol_symbols = protocol_category_summary.resolved_protocol_symbols;
  ir_frontend_metadata.resolved_category_symbols = protocol_category_summary.resolved_category_symbols;
  ir_frontend_metadata.protocol_method_symbols = protocol_category_summary.protocol_method_symbols;
  ir_frontend_metadata.category_method_symbols = protocol_category_summary.category_method_symbols;
  ir_frontend_metadata.linked_category_symbols = protocol_category_summary.linked_category_symbols;
  ir_frontend_metadata.declared_class_interfaces = class_protocol_category_linking_summary.declared_class_interfaces;
  ir_frontend_metadata.declared_class_implementations =
      class_protocol_category_linking_summary.declared_class_implementations;
  ir_frontend_metadata.resolved_class_interfaces = class_protocol_category_linking_summary.resolved_class_interfaces;
  ir_frontend_metadata.resolved_class_implementations =
      class_protocol_category_linking_summary.resolved_class_implementations;
  ir_frontend_metadata.linked_class_method_symbols =
      class_protocol_category_linking_summary.linked_class_method_symbols;
  ir_frontend_metadata.linked_category_method_symbols =
      class_protocol_category_linking_summary.linked_category_method_symbols;
  ir_frontend_metadata.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  ir_frontend_metadata.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  ir_frontend_metadata.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  ir_frontend_metadata.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  ir_frontend_metadata.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  ir_frontend_metadata.selector_method_declaration_entries = selector_normalization_summary.method_declaration_entries;
  ir_frontend_metadata.selector_normalized_method_declarations =
      selector_normalization_summary.normalized_method_declarations;
  ir_frontend_metadata.selector_piece_entries = selector_normalization_summary.selector_piece_entries;
  ir_frontend_metadata.selector_piece_parameter_links = selector_normalization_summary.selector_piece_parameter_links;
  ir_frontend_metadata.property_declaration_entries = property_attribute_summary.property_declaration_entries;
  ir_frontend_metadata.property_attribute_entries = property_attribute_summary.property_attribute_entries;
  ir_frontend_metadata.property_attribute_value_entries = property_attribute_summary.property_attribute_value_entries;
  ir_frontend_metadata.property_accessor_modifier_entries = property_attribute_summary.property_accessor_modifier_entries;
  ir_frontend_metadata.property_getter_selector_entries = property_attribute_summary.property_getter_selector_entries;
  ir_frontend_metadata.property_setter_selector_entries = property_attribute_summary.property_setter_selector_entries;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =
      property_synthesis_ivar_binding_replay_key;
  ir_frontend_metadata.lowering_property_synthesis_sites =
      property_synthesis_ivar_binding_contract.property_synthesis_sites;
  ir_frontend_metadata.lowering_property_synthesis_explicit_ivar_bindings =
      property_synthesis_ivar_binding_contract.property_synthesis_explicit_ivar_bindings;
  ir_frontend_metadata.lowering_property_synthesis_default_ivar_bindings =
      property_synthesis_ivar_binding_contract.property_synthesis_default_ivar_bindings;
  ir_frontend_metadata.lowering_interface_owned_property_synthesis_sites =
      property_synthesis_ivar_binding_contract
          .interface_owned_property_synthesis_sites;
  ir_frontend_metadata.lowering_implementation_property_redeclaration_sites =
      property_synthesis_ivar_binding_contract
          .implementation_property_redeclaration_sites;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_resolved =
      property_synthesis_ivar_binding_contract.ivar_binding_resolved;
  ir_frontend_metadata.lowering_property_synthesis_deterministic_handoff =
      property_synthesis_ivar_binding_contract.deterministic;
  ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =
      id_class_sel_object_pointer_typecheck_replay_key;
  ir_frontend_metadata.id_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites;
  ir_frontend_metadata.class_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites;
  ir_frontend_metadata.sel_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites;
  ir_frontend_metadata.object_pointer_typecheck_sites =
      id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites;
  ir_frontend_metadata.id_class_sel_object_pointer_typecheck_sites_total =
      id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites;
  ir_frontend_metadata.lowering_dispatch_surface_classification_replay_key =
      dispatch_surface_classification_replay_key;
  ir_frontend_metadata.dispatch_surface_classification_instance_sites =
      dispatch_surface_classification_contract.instance_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_class_sites =
      dispatch_surface_classification_contract.class_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_super_sites =
      dispatch_surface_classification_contract.super_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_direct_sites =
      dispatch_surface_classification_contract.direct_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_sites =
      dispatch_surface_classification_contract.dynamic_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_instance_entrypoint_family =
      dispatch_surface_classification_contract.instance_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_class_entrypoint_family =
      dispatch_surface_classification_contract.class_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_super_entrypoint_family =
      dispatch_surface_classification_contract.super_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_direct_entrypoint_family =
      dispatch_surface_classification_contract.direct_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_entrypoint_family =
      dispatch_surface_classification_contract.dynamic_entrypoint_family;
  ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =
      message_send_selector_lowering_replay_key;
  ir_frontend_metadata.message_send_selector_lowering_sites =
      message_send_selector_lowering_contract.message_send_sites;
  ir_frontend_metadata.message_send_selector_lowering_unary_sites =
      message_send_selector_lowering_contract.unary_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_keyword_sites =
      message_send_selector_lowering_contract.keyword_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_piece_sites =
      message_send_selector_lowering_contract.selector_piece_sites;
  ir_frontend_metadata.message_send_selector_lowering_argument_expression_sites =
      message_send_selector_lowering_contract.argument_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_receiver_sites =
      message_send_selector_lowering_contract.receiver_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_entries =
      message_send_selector_lowering_contract.selector_literal_entries;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_characters =
      message_send_selector_lowering_contract.selector_literal_characters;
  ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key = dispatch_abi_marshalling_replay_key;
  ir_frontend_metadata.dispatch_abi_marshalling_message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  ir_frontend_metadata.dispatch_abi_marshalling_receiver_slots_marshaled =
      dispatch_abi_marshalling_contract.receiver_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_selector_slots_marshaled =
      dispatch_abi_marshalling_contract.selector_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_value_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_value_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_padding_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_padding_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_total_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_total_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_total_marshaled_slots =
      dispatch_abi_marshalling_contract.total_marshaled_slots;
  ir_frontend_metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots =
      dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =
      nil_receiver_semantics_foldability_replay_key;
  ir_frontend_metadata.nil_receiver_semantics_foldability_message_send_sites =
      nil_receiver_semantics_foldability_contract.message_send_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites =
      nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_enabled_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_foldable_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites =
      nil_receiver_semantics_foldability_contract.non_nil_receiver_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_contract_violation_sites =
      nil_receiver_semantics_foldability_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =
      super_dispatch_method_family_replay_key;
  ir_frontend_metadata.super_dispatch_method_family_message_send_sites =
      super_dispatch_method_family_contract.message_send_sites;
  ir_frontend_metadata.super_dispatch_method_family_receiver_super_identifier_sites =
      super_dispatch_method_family_contract.receiver_super_identifier_sites;
  ir_frontend_metadata.super_dispatch_method_family_enabled_sites =
      super_dispatch_method_family_contract.super_dispatch_enabled_sites;
  ir_frontend_metadata.super_dispatch_method_family_requires_class_context_sites =
      super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites;
  ir_frontend_metadata.super_dispatch_method_family_init_sites =
      super_dispatch_method_family_contract.method_family_init_sites;
  ir_frontend_metadata.super_dispatch_method_family_copy_sites =
      super_dispatch_method_family_contract.method_family_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_mutable_copy_sites =
      super_dispatch_method_family_contract.method_family_mutable_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_new_sites =
      super_dispatch_method_family_contract.method_family_new_sites;
  ir_frontend_metadata.super_dispatch_method_family_none_sites =
      super_dispatch_method_family_contract.method_family_none_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_retained_result_sites =
      super_dispatch_method_family_contract.method_family_returns_retained_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_related_result_sites =
      super_dispatch_method_family_contract.method_family_returns_related_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_contract_violation_sites =
      super_dispatch_method_family_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_runtime_link_host_link_replay_key = runtime_link_host_link_replay_key;
  ir_frontend_metadata.runtime_link_host_link_message_send_sites =
      runtime_link_host_link_contract.message_send_sites;
  ir_frontend_metadata.runtime_link_host_link_required_sites =
      runtime_link_host_link_contract.runtime_link_required_sites;
  ir_frontend_metadata.runtime_link_host_link_elided_sites =
      runtime_link_host_link_contract.runtime_link_elided_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_arg_slots =
      runtime_link_host_link_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_declaration_parameter_count =
      runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count;
  ir_frontend_metadata.runtime_link_host_link_contract_violation_sites =
      runtime_link_host_link_contract.contract_violation_sites;
  ir_frontend_metadata.runtime_link_host_link_runtime_dispatch_symbol =
      runtime_link_host_link_contract.runtime_dispatch_symbol;
  ir_frontend_metadata.runtime_link_host_link_default_runtime_dispatch_symbol_binding =
      runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding;
  ir_frontend_metadata.lowering_async_continuation_replay_key =
      concurrency_async_continuation_lowering_replay_key;
  ir_frontend_metadata.async_continuation_lowering_sites =
      concurrency_async_continuation_lowering_contract.async_continuation_sites;
  ir_frontend_metadata.async_continuation_lowering_async_keyword_sites =
      concurrency_async_continuation_lowering_contract.async_keyword_sites;
  ir_frontend_metadata.async_continuation_lowering_async_function_sites =
      concurrency_async_continuation_lowering_contract.async_function_sites;
  ir_frontend_metadata
      .async_continuation_lowering_continuation_allocation_sites =
      concurrency_async_continuation_lowering_contract.continuation_allocation_sites;
  ir_frontend_metadata.async_continuation_lowering_continuation_resume_sites =
      concurrency_async_continuation_lowering_contract.continuation_resume_sites;
  ir_frontend_metadata.async_continuation_lowering_continuation_suspend_sites =
      concurrency_async_continuation_lowering_contract.continuation_suspend_sites;
  ir_frontend_metadata.async_continuation_lowering_async_state_machine_sites =
      concurrency_async_continuation_lowering_contract.async_state_machine_sites;
  ir_frontend_metadata.async_continuation_lowering_normalized_sites =
      concurrency_async_continuation_lowering_contract.normalized_sites;
  ir_frontend_metadata.async_continuation_lowering_gate_blocked_sites =
      concurrency_async_continuation_lowering_contract.gate_blocked_sites;
  ir_frontend_metadata.async_continuation_lowering_contract_violation_sites =
      concurrency_async_continuation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_async_continuation_lowering_handoff =
      concurrency_async_continuation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_await_lowering_suspension_state_replay_key =
      concurrency_await_lowering_suspension_state_lowering_replay_key;
  ir_frontend_metadata.await_lowering_suspension_state_lowering_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_suspension_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_keyword_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_keyword_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_suspension_point_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_suspension_point_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_resume_sites =
      concurrency_await_lowering_suspension_state_lowering_contract.await_resume_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_state_machine_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_state_machine_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_continuation_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .await_continuation_sites;
  ir_frontend_metadata.await_lowering_suspension_state_lowering_normalized_sites =
      concurrency_await_lowering_suspension_state_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_gate_blocked_sites =
      concurrency_await_lowering_suspension_state_lowering_contract.gate_blocked_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_contract_violation_sites =
      concurrency_await_lowering_suspension_state_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      concurrency_await_lowering_suspension_state_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_actor_isolation_sendability_replay_key =
      concurrency_actor_isolation_sendability_lowering_replay_key;
  ir_frontend_metadata.actor_isolation_sendability_lowering_sites =
      concurrency_actor_isolation_sendability_lowering_contract.actor_isolation_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_sendability_check_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .sendability_check_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_cross_actor_hop_sites =
      concurrency_actor_isolation_sendability_lowering_contract.cross_actor_hop_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_non_sendable_capture_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .non_sendable_capture_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_sendable_transfer_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .sendable_transfer_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_isolation_boundary_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .isolation_boundary_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_guard_blocked_sites =
      concurrency_actor_isolation_sendability_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_contract_violation_sites =
      concurrency_actor_isolation_sendability_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata.deterministic_actor_isolation_sendability_lowering_handoff =
      concurrency_actor_isolation_sendability_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_actor_lowering_metadata_replay_key =
      concurrency_actor_lowering_metadata_replay_key;
  // implementation anchor: the deterministic actor-lowering
  // contract now feeds helper-backed actor thunk/hop/nonisolated rewrites in
  // IR rather than staying metadata-only.
  ir_frontend_metadata.actor_lowering_metadata_actor_interface_sites =
      concurrency_actor_lowering_metadata_contract.actor_interface_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_method_sites =
      concurrency_actor_lowering_metadata_contract.actor_method_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_metadata_record_sites =
      concurrency_actor_lowering_metadata_contract.actor_metadata_record_sites;
  ir_frontend_metadata.actor_lowering_metadata_nonisolated_entry_sites =
      concurrency_actor_lowering_metadata_contract.nonisolated_entry_sites;
  ir_frontend_metadata.actor_lowering_metadata_executor_affinity_sites =
      concurrency_actor_lowering_metadata_contract.executor_affinity_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_hop_artifact_sites =
      concurrency_actor_lowering_metadata_contract.actor_hop_artifact_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_isolation_thunk_sites =
      concurrency_actor_lowering_metadata_contract.actor_isolation_thunk_sites;
  ir_frontend_metadata.actor_lowering_metadata_replay_proof_dependency_sites =
      concurrency_actor_lowering_metadata_contract.replay_proof_dependency_sites;
  ir_frontend_metadata.actor_lowering_metadata_race_guard_dependency_sites =
      concurrency_actor_lowering_metadata_contract.race_guard_dependency_sites;
  ir_frontend_metadata.actor_lowering_metadata_task_handoff_sites =
      concurrency_actor_lowering_metadata_contract.task_handoff_sites;
  ir_frontend_metadata.actor_lowering_metadata_guard_blocked_sites =
      concurrency_actor_lowering_metadata_contract.guard_blocked_sites;
  ir_frontend_metadata.actor_lowering_metadata_contract_violation_sites =
      concurrency_actor_lowering_metadata_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_actor_lowering_metadata_handoff =
      concurrency_actor_lowering_metadata_contract.deterministic;
  ir_frontend_metadata.lowering_dispatch_dispatch_control_replay_key =
      dispatch_dispatch_control_lowering_replay_key;
  ir_frontend_metadata.lowering_metaprogramming_expansion_replay_key =
      metaprogramming_expansion_lowering_replay_key;
  ir_frontend_metadata.metaprogramming_expansion_lowering_derive_inventory_sites =
      metaprogramming_expansion_lowering_contract.derive_inventory_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_derived_selector_artifact_sites =
      metaprogramming_expansion_lowering_contract.derived_selector_artifact_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_macro_replay_visible_sites =
      metaprogramming_expansion_lowering_contract.macro_replay_visible_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_property_behavior_sites =
      metaprogramming_expansion_lowering_contract.property_behavior_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_synthesized_binding_sites =
      metaprogramming_expansion_lowering_contract.synthesized_binding_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_synthesized_getter_sites =
      metaprogramming_expansion_lowering_contract.synthesized_getter_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_synthesized_setter_sites =
      metaprogramming_expansion_lowering_contract.synthesized_setter_sites;
  ir_frontend_metadata
      .metaprogramming_expansion_lowering_replay_visible_metadata_sites =
      metaprogramming_expansion_lowering_contract.replay_visible_metadata_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_guard_blocked_sites =
      metaprogramming_expansion_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.metaprogramming_expansion_lowering_contract_violation_sites =
      metaprogramming_expansion_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_metaprogramming_expansion_lowering_handoff =
      metaprogramming_expansion_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_metaprogramming_synthesized_emission_replay_key =
      metaprogramming_synthesized_artifact_emission_replay_key;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_derive_method_sites =
      metaprogramming_synthesized_artifact_emission_contract.emitted_derive_method_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_macro_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract.emitted_macro_artifact_sites;
  ir_frontend_metadata
      .metaprogramming_synthesized_emitted_property_behavior_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_property_behavior_artifact_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_global_artifact_sites =
      metaprogramming_synthesized_artifact_emission_contract.emitted_global_artifact_sites;
  ir_frontend_metadata.metaprogramming_synthesized_emitted_runtime_method_list_sites =
      metaprogramming_synthesized_artifact_emission_contract
          .emitted_runtime_method_list_sites;
  ir_frontend_metadata.metaprogramming_synthesized_guard_blocked_sites =
      metaprogramming_synthesized_artifact_emission_contract.guard_blocked_sites;
  ir_frontend_metadata.metaprogramming_synthesized_contract_violation_sites =
      metaprogramming_synthesized_artifact_emission_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_metaprogramming_synthesized_emission_handoff =
      metaprogramming_synthesized_artifact_emission_contract.deterministic;
  ir_frontend_metadata.metaprogramming_derived_method_bundles_lexicographic =
      metaprogramming_derived_method_bundles;
  ir_frontend_metadata.metaprogramming_macro_artifact_bundles_lexicographic =
      metaprogramming_macro_artifact_bundles;
  ir_frontend_metadata.metaprogramming_property_behavior_artifact_bundles_lexicographic =
      metaprogramming_property_behavior_artifact_bundles;
  ir_frontend_metadata.lowering_metaprogramming_module_interface_replay_preservation_key =
      metaprogramming_module_interface_replay_preservation_summary.replay_key;
  ir_frontend_metadata.metaprogramming_module_replay_local_derive_method_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_derive_method_count;
  ir_frontend_metadata.metaprogramming_module_replay_local_macro_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_macro_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_interface_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_interface_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_local_implementation_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_implementation_property_behavior_artifact_count;
  ir_frontend_metadata.metaprogramming_module_replay_local_runtime_method_list_count =
      metaprogramming_module_interface_replay_preservation_summary
          .local_runtime_method_list_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_module_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_module_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_derive_method_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_derive_method_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_macro_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_macro_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_interface_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_interface_property_behavior_artifact_count;
  ir_frontend_metadata
      .metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_implementation_property_behavior_artifact_count;
  ir_frontend_metadata.metaprogramming_module_replay_imported_runtime_method_list_count =
      metaprogramming_module_interface_replay_preservation_summary
          .imported_runtime_method_list_count;
  ir_frontend_metadata.metaprogramming_module_replay_runtime_import_artifact_ready =
      metaprogramming_module_interface_replay_preservation_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .metaprogramming_module_replay_separate_compilation_preservation_ready =
      metaprogramming_module_interface_replay_preservation_summary
          .separate_compilation_preservation_ready;
  ir_frontend_metadata.deterministic_metaprogramming_module_interface_replay_handoff =
      metaprogramming_module_interface_replay_preservation_summary.deterministic;
  ir_frontend_metadata.lowering_interop_interop_replay_key =
      interop_interop_lowering_replay_key;
  ir_frontend_metadata.interop_interop_lowering_foreign_callable_sites =
      interop_interop_lowering_contract.foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_c_foreign_callable_sites =
      interop_interop_lowering_contract.c_foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_objc_runtime_parity_callable_sites =
      interop_interop_lowering_contract.objc_runtime_parity_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_ownership_bridge_callable_sites =
      interop_interop_lowering_contract.ownership_bridge_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_error_surface_sites =
      interop_interop_lowering_contract.error_surface_sites;
  ir_frontend_metadata.interop_interop_lowering_async_boundary_sites =
      interop_interop_lowering_contract.async_boundary_sites;
  ir_frontend_metadata.interop_interop_lowering_swift_concurrency_metadata_sites =
      interop_interop_lowering_contract.swift_concurrency_metadata_sites;
  ir_frontend_metadata.interop_interop_lowering_interface_preserved_foreign_callable_sites =
      interop_interop_lowering_contract.interface_preserved_foreign_callable_sites;
  ir_frontend_metadata.interop_interop_lowering_interface_preserved_metadata_annotation_sites =
      interop_interop_lowering_contract.interface_preserved_metadata_annotation_sites;
  ir_frontend_metadata.interop_interop_lowering_guard_blocked_sites =
      interop_interop_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.interop_interop_lowering_contract_violation_sites =
      interop_interop_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_interop_interop_lowering_handoff =
      interop_interop_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_interop_foreign_call_lifetime_replay_key =
      interop_foreign_call_lifetime_lowering_replay_key;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_foreign_callable_sites =
      interop_foreign_call_lifetime_lowering_contract.foreign_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_c_foreign_callable_sites =
      interop_foreign_call_lifetime_lowering_contract.c_foreign_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites =
      interop_foreign_call_lifetime_lowering_contract
          .objc_runtime_parity_callable_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_ownership_bridge_sites =
      interop_foreign_call_lifetime_lowering_contract.ownership_bridge_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_lifetime_bridge_sites =
      interop_foreign_call_lifetime_lowering_contract.lifetime_bridge_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_metadata_preservation_sites =
      interop_foreign_call_lifetime_lowering_contract
          .metadata_preservation_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_guard_blocked_sites =
      interop_foreign_call_lifetime_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .interop_foreign_call_lifetime_lowering_contract_violation_sites =
      interop_foreign_call_lifetime_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_interop_foreign_call_lifetime_lowering_handoff =
      interop_foreign_call_lifetime_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_interop_ffi_metadata_interface_preservation_key =
      interop_ffi_metadata_interface_preservation_replay_key;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_foreign_callable_count =
      interop_ffi_metadata_interface_preservation_contract
          .local_foreign_callable_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .local_metadata_preservation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_local_interface_annotation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .local_interface_annotation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_module_count =
      interop_ffi_metadata_interface_preservation_contract.imported_module_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_foreign_callable_count =
      interop_ffi_metadata_interface_preservation_contract
          .imported_foreign_callable_count;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .imported_metadata_preservation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites =
      interop_ffi_metadata_interface_preservation_contract
          .imported_interface_annotation_sites;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready =
      interop_ffi_metadata_interface_preservation_contract
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready =
      interop_ffi_metadata_interface_preservation_contract
          .separate_compilation_preservation_ready;
  ir_frontend_metadata
      .deterministic_interop_ffi_metadata_interface_preservation_handoff =
      interop_ffi_metadata_interface_preservation_contract.deterministic;
  ir_frontend_metadata.lowering_interop_header_module_bridge_generation_key =
      interop_header_module_bridge_generation_summary.replay_key;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_direct_call_candidate_sites =
      dispatch_dispatch_control_lowering_contract.direct_call_candidate_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      dispatch_dispatch_control_lowering_contract.direct_members_defaulted_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_dynamic_opt_out_sites =
      dispatch_dispatch_control_lowering_contract.dynamic_opt_out_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_final_container_sites =
      dispatch_dispatch_control_lowering_contract.final_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_sealed_container_sites =
      dispatch_dispatch_control_lowering_contract.sealed_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_override_legality_sites =
      dispatch_dispatch_control_lowering_contract.override_legality_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      dispatch_dispatch_control_lowering_contract
          .metadata_preserved_callable_sites;
  ir_frontend_metadata
      .dispatch_dispatch_control_lowering_metadata_preserved_container_sites =
      dispatch_dispatch_control_lowering_contract
          .metadata_preserved_container_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_guard_blocked_sites =
      dispatch_dispatch_control_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.dispatch_dispatch_control_lowering_contract_violation_sites =
      dispatch_dispatch_control_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_dispatch_dispatch_control_lowering_handoff =
      dispatch_dispatch_control_lowering_contract.deterministic;
  ir_frontend_metadata
      .lowering_dispatch_dispatch_metadata_interface_preservation_key =
      dispatch_dispatch_metadata_interface_preservation_summary.replay_key;
  ir_frontend_metadata.dispatch_dispatch_metadata_local_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_direct_callable_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_local_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_final_callable_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_local_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_local_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .local_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_module_count =
      dispatch_dispatch_metadata_interface_preservation_summary.imported_module_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_direct_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_direct_callable_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_imported_final_callable_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_final_callable_record_count;
