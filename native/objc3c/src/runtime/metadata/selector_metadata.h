#pragma once

struct Objc3RuntimeTranslationUnitRegistrationManifestSummary {
  std::string contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string launch_integration_contract_id =
      "objc3c.runtime.launch.integration.v1";
  std::string translation_unit_registration_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string manifest_surface_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestSurfacePath;
  std::string manifest_payload_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestPayloadModel;
  std::string manifest_artifact_relative_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath;
  std::array<std::string, 3u> runtime_owned_payload_artifacts = {
      kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath};
  std::string runtime_support_library_archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_root_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string constructor_init_stub_symbol_prefix =
      kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix;
  std::string constructor_init_stub_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationInitStubOwnershipModel;
  std::string constructor_priority_policy =
      kObjc3RuntimeTranslationUnitRegistrationManifestPriorityPolicy;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string runtime_library_resolution_model =
      "registration-manifest-runtime-archive-path-is-authoritative";
  std::string driver_linker_flag_consumption_model =
      "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands";
  std::string compile_wrapper_command_surface =
      "scripts/objc3c_native_compile.ps1";
  std::string compile_proof_command_surface =
      "scripts/run_objc3c_native_compile_proof.ps1";
  std::string execution_smoke_command_surface =
      "scripts/check_objc3c_native_execution_smoke.ps1";
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool translation_unit_registration_contract_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool runtime_manifest_template_published = false;
  bool constructor_root_manifest_authoritative = false;
  bool constructor_root_reserved_for_lowering = false;
  bool init_stub_emission_deferred_to_lowering = false;
  bool runtime_registration_artifact_emitted_by_driver = false;
  bool ready_for_lowering_init_stub_emission = false;
  bool launch_integration_ready = false;
  std::size_t runtime_owned_payload_artifact_count = 0;
  std::string translation_unit_registration_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.launch_integration_contract_id.empty() &&
         !summary.translation_unit_registration_contract_id.empty() &&
         !summary.runtime_support_library_link_wiring_contract_id.empty() &&
         !summary.manifest_surface_path.empty() &&
         !summary.manifest_payload_model.empty() &&
         !summary.manifest_artifact_relative_path.empty() &&
         summary.fail_closed &&
         summary.translation_unit_registration_contract_ready &&
         summary.runtime_support_library_link_wiring_ready &&
         summary.runtime_manifest_template_published &&
         summary.constructor_root_manifest_authoritative &&
         summary.constructor_root_reserved_for_lowering &&
         summary.init_stub_emission_deferred_to_lowering &&
         summary.runtime_registration_artifact_emitted_by_driver &&
         summary.ready_for_lowering_init_stub_emission &&
         summary.launch_integration_ready &&
         summary.runtime_owned_payload_artifact_count ==
             summary.runtime_owned_payload_artifacts.size() &&
         !summary.runtime_support_library_archive_relative_path.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_root_ownership_model.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.constructor_init_stub_symbol_prefix.empty() &&
         !summary.constructor_init_stub_ownership_model.empty() &&
         !summary.constructor_priority_policy.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.runtime_library_resolution_model.empty() &&
         !summary.driver_linker_flag_consumption_model.empty() &&
         !summary.compile_wrapper_command_surface.empty() &&
         !summary.compile_proof_command_surface.empty() &&
         !summary.execution_smoke_command_surface.empty() &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         !summary.translation_unit_registration_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeStartupBootstrapInvariantSummary {
  std::string contract_id = kObjc3RuntimeStartupBootstrapInvariantContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_surface_path =
      kObjc3RuntimeStartupBootstrapInvariantSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string realization_order_policy =
      kObjc3RuntimeStartupBootstrapRealizationOrderPolicy;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string image_local_initialization_scope =
      kObjc3RuntimeStartupBootstrapImageLocalInitializationScope;
  std::string constructor_root_uniqueness_policy =
      kObjc3RuntimeStartupBootstrapConstructorRootUniquenessPolicy;
  std::string constructor_root_consumption_model =
      kObjc3RuntimeStartupBootstrapConsumptionModel;
  std::string startup_execution_mode =
      kObjc3RuntimeStartupBootstrapExecutionMode;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool duplicate_registration_semantics_frozen = false;
  bool realization_order_semantics_frozen = false;
  bool failure_mode_semantics_frozen = false;
  bool image_local_initialization_scope_frozen = false;
  bool constructor_root_uniqueness_frozen = false;
  bool startup_execution_not_yet_landed = false;
  bool live_duplicate_registration_enforcement_not_yet_landed = false;
  bool image_local_realization_not_yet_landed = false;
  bool ready_for_bootstrap_implementation = false;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.realization_order_policy.empty() &&
         !summary.failure_mode.empty() &&
         !summary.image_local_initialization_scope.empty() &&
         !summary.constructor_root_uniqueness_policy.empty() &&
         !summary.constructor_root_consumption_model.empty() &&
         !summary.startup_execution_mode.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.duplicate_registration_semantics_frozen &&
         summary.realization_order_semantics_frozen &&
         summary.failure_mode_semantics_frozen &&
         summary.image_local_initialization_scope_frozen &&
         summary.constructor_root_uniqueness_frozen &&
         summary.startup_execution_not_yet_landed &&
         summary.live_duplicate_registration_enforcement_not_yet_landed &&
         summary.image_local_realization_not_yet_landed &&
         summary.ready_for_bootstrap_implementation &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapSemanticsSummary {
  std::string contract_id = kObjc3RuntimeBootstrapSemanticsContractId;
  std::string bootstrap_invariant_contract_id =
      kObjc3RuntimeStartupBootstrapInvariantContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_surface_path =
      kObjc3RuntimeBootstrapSemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string realization_order_policy =
      kObjc3RuntimeStartupBootstrapRealizationOrderPolicy;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string image_local_initialization_scope =
      kObjc3RuntimeStartupBootstrapImageLocalInitializationScope;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string manifest_authority_model =
      kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string runtime_library_archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string registration_result_model =
      kObjc3RuntimeBootstrapResultModel;
  std::string registration_order_ordinal_model =
      kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  int success_status_code = kObjc3RuntimeBootstrapSuccessStatusCode;
  int invalid_descriptor_status_code =
      kObjc3RuntimeBootstrapInvalidDescriptorStatusCode;
  int duplicate_registration_status_code =
      kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode;
  int out_of_order_status_code =
      kObjc3RuntimeBootstrapOutOfOrderStatusCode;
  int invalid_registration_roots_status_code =
      kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool bootstrap_invariant_contract_ready = false;
  bool registration_manifest_contract_ready = false;
  bool live_runtime_enforcement_landed = false;
  bool registration_manifest_bootstrap_semantics_published = false;
  bool runtime_probe_required = false;
  bool no_partial_commit_on_failure = false;
  bool ready_for_constructor_root_implementation = false;
  std::string bootstrap_invariant_replay_key;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_invariant_contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.realization_order_policy.empty() &&
         !summary.failure_mode.empty() &&
         !summary.image_local_initialization_scope.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.manifest_authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.runtime_library_archive_relative_path.empty() &&
         !summary.registration_result_model.empty() &&
         !summary.registration_order_ordinal_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed &&
         summary.bootstrap_invariant_contract_ready &&
         summary.registration_manifest_contract_ready &&
         summary.live_runtime_enforcement_landed &&
         summary.registration_manifest_bootstrap_semantics_published &&
         summary.runtime_probe_required &&
         summary.no_partial_commit_on_failure &&
         summary.ready_for_constructor_root_implementation &&
         !summary.bootstrap_invariant_replay_key.empty() &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLoweringSummary {
  std::string contract_id = kObjc3RuntimeBootstrapLoweringContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId;
  std::string registration_descriptor_artifact =
      kObjc3RuntimeBootstrapRegistrationDescriptorArtifact;
  std::string bootstrap_surface_path =
      "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract";
  std::string lowering_boundary_model =
      kObjc3RuntimeBootstrapLoweringBoundaryModel;
  std::string registration_descriptor_handoff_model =
      kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel;
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_init_stub_symbol_prefix =
      kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix;
  std::string registration_table_symbol_prefix =
      kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix;
  std::string image_local_init_state_symbol_prefix =
      kObjc3RuntimeBootstrapImageLocalInitStateSymbolPrefix;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string global_ctor_list_model =
      kObjc3RuntimeBootstrapGlobalCtorListModel;
  std::string registration_table_layout_model =
      kObjc3RuntimeBootstrapRegistrationTableLayoutModel;
  std::string image_local_initialization_model =
      kObjc3RuntimeBootstrapImageLocalInitializationModel;
  std::uint64_t registration_table_abi_version =
      kObjc3RuntimeBootstrapRegistrationTableAbiVersion;
  std::uint64_t registration_table_pointer_field_count =
      kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount;
  std::string constructor_root_emission_state =
      kObjc3RuntimeBootstrapConstructorRootEmissionState;
  std::string init_stub_emission_state =
      kObjc3RuntimeBootstrapInitStubEmissionState;
  std::string registration_table_emission_state =
      kObjc3RuntimeBootstrapRegistrationTableEmissionState;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool lowering_contract_published = false;
  bool manifest_authority_preserved = false;
  bool no_bootstrap_ir_materialization_yet = false;
  bool bootstrap_ir_materialization_landed = false;
  bool image_local_initialization_landed = false;
  bool ready_for_bootstrap_materialization = false;
  std::string registration_manifest_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLoweringSummary(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.registration_descriptor_artifact.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.lowering_boundary_model.empty() &&
         !summary.registration_descriptor_handoff_model.empty() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_init_stub_symbol_prefix.empty() &&
         !summary.registration_table_symbol_prefix.empty() &&
         !summary.image_local_init_state_symbol_prefix.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.global_ctor_list_model.empty() &&
         !summary.registration_table_layout_model.empty() &&
         !summary.image_local_initialization_model.empty() &&
         summary.registration_table_abi_version > 0 &&
         summary.registration_table_pointer_field_count > 0 &&
         !summary.constructor_root_emission_state.empty() &&
         !summary.init_stub_emission_state.empty() &&
         !summary.registration_table_emission_state.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.lowering_contract_published &&
         summary.manifest_authority_preserved &&
         !summary.no_bootstrap_ir_materialization_yet &&
         summary.bootstrap_ir_materialization_landed &&
         summary.image_local_initialization_landed &&
         summary.ready_for_bootstrap_materialization &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary {
  std::string contract_id =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_surface_path =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath;
  std::string registration_descriptor_pragma_name =
      kObjc3BootstrapRegistrationDescriptorPragmaName;
  std::string image_root_pragma_name = kObjc3BootstrapImageRootPragmaName;
  std::string module_identity_source =
      kObjc3RuntimeBootstrapModuleIdentitySourceModel;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string bootstrap_visible_metadata_ownership_model =
      kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel;
  std::string module_name = "objc3_module";
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") + kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool source_surface_frozen = false;
  bool prelude_pragma_contract_published = false;
  bool registration_descriptor_identifier_resolved = false;
  bool image_root_identifier_resolved = false;
  bool bootstrap_visible_metadata_ownership_published = false;
  bool ready_for_descriptor_frontend_closure = false;
  bool registration_descriptor_pragma_seen = false;
  bool registration_descriptor_pragma_duplicate = false;
  bool registration_descriptor_pragma_non_leading = false;
  std::size_t registration_descriptor_pragma_directive_count = 0;
  bool image_root_pragma_seen = false;
  bool image_root_pragma_duplicate = false;
  bool image_root_pragma_non_leading = false;
  std::size_t image_root_pragma_directive_count = 0;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.source_surface_path.empty() &&
         !summary.registration_descriptor_pragma_name.empty() &&
         !summary.image_root_pragma_name.empty() &&
         !summary.module_identity_source.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         !summary.bootstrap_visible_metadata_ownership_model.empty() &&
         !summary.module_name.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.source_surface_frozen &&
         summary.prelude_pragma_contract_published &&
         summary.registration_descriptor_identifier_resolved &&
         summary.image_root_identifier_resolved &&
         summary.bootstrap_visible_metadata_ownership_published &&
         summary.ready_for_descriptor_frontend_closure &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary {
  std::string contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_surface_contract_id =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath;
  std::string payload_model =
      kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel;
  std::string artifact_relative_path =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string payload_ownership_model =
      kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string bootstrap_visible_metadata_ownership_model =
      kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool source_surface_contract_ready = false;
  bool registration_manifest_contract_ready = false;
  bool descriptor_frontend_surface_published = false;
  bool descriptor_artifact_template_published = false;
  bool descriptor_fields_resolved = false;
  bool ready_for_descriptor_artifact_emission = false;
  bool ready_for_registration_descriptor_lowering = false;
  std::string source_surface_replay_key;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.source_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.payload_ownership_model.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         !summary.bootstrap_visible_metadata_ownership_model.empty() &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.source_surface_contract_ready &&
         summary.registration_manifest_contract_ready &&
         summary.descriptor_frontend_surface_published &&
         summary.descriptor_artifact_template_published &&
         summary.descriptor_fields_resolved &&
         summary.ready_for_descriptor_artifact_emission &&
         summary.ready_for_registration_descriptor_lowering &&
         !summary.source_surface_replay_key.empty() &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLegalityFailureContractSummary {
  std::string contract_id = kObjc3BootstrapLegalityFailureContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapLegalityFailureSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool semantic_boundary_ready = false;
  bool duplicate_registration_policy_frozen = false;
  bool image_order_invariant_frozen = false;
  bool bootstrap_rejection_frozen = false;
  bool restart_boundary_frozen = false;
  bool semantic_diagnostics_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLegalityFailureContractSummary(
    const Objc3RuntimeBootstrapLegalityFailureContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.semantic_boundary_ready &&
         summary.duplicate_registration_policy_frozen &&
         summary.image_order_invariant_frozen &&
         summary.bootstrap_rejection_frozen &&
         summary.restart_boundary_frozen &&
         summary.semantic_diagnostics_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapLegalitySemanticsSummary {
  std::string contract_id = kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_legality_failure_contract_id =
      kObjc3BootstrapLegalityFailureContractId;
  std::string registration_descriptor_frontend_closure_contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapLegalitySemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string cross_image_legality_model =
      kObjc3BootstrapLegalityCrossImageLegalityModel;
  std::string semantic_diagnostic_model =
      kObjc3BootstrapLegalitySemanticDiagnosticModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string translation_unit_identity_key;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool bootstrap_legality_failure_contract_ready = false;
  bool registration_descriptor_frontend_closure_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool duplicate_registration_semantics_landed = false;
  bool image_order_semantics_landed = false;
  bool cross_image_legality_semantics_landed = false;
  bool semantic_diagnostics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string bootstrap_legality_failure_contract_replay_key;
  std::string registration_descriptor_frontend_closure_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapLegalitySemanticsSummary(
    const Objc3RuntimeBootstrapLegalitySemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_failure_contract_id.empty() &&
         !summary.registration_descriptor_frontend_closure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.cross_image_legality_model.empty() &&
         !summary.semantic_diagnostic_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_identity_key.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.semantic_boundary_ready &&
         summary.bootstrap_legality_failure_contract_ready &&
         summary.registration_descriptor_frontend_closure_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.duplicate_registration_semantics_landed &&
         summary.image_order_semantics_landed &&
         summary.cross_image_legality_semantics_landed &&
         summary.semantic_diagnostics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.bootstrap_legality_failure_contract_replay_key.empty() &&
         !summary.registration_descriptor_frontend_closure_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary {
  std::string contract_id = kObjc3BootstrapFailureRestartSemanticsContractId;
  std::string bootstrap_legality_semantics_contract_id =
      kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_reset_contract_id =
      kObjc3RuntimeBootstrapResetContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3BootstrapFailureRestartSemanticsSurfacePath;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string unsupported_topology_model =
      kObjc3BootstrapFailureRestartUnsupportedTopologyModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string translation_unit_identity_key;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string replay_registered_images_symbol =
      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
  std::string reset_replay_state_snapshot_symbol =
      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
  int invalid_descriptor_status_code =
      kObjc3RuntimeBootstrapInvalidDescriptorStatusCode;
  int invalid_registration_roots_status_code =
      kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool bootstrap_legality_semantics_contract_ready = false;
  bool bootstrap_semantics_contract_ready = false;
  bool bootstrap_reset_contract_ready = false;
  bool failure_mode_semantics_landed = false;
  bool restart_semantics_landed = false;
  bool replay_semantics_landed = false;
  bool unsupported_topology_semantics_landed = false;
  bool deterministic_recovery_semantics_landed = false;
  bool runtime_restart_probe_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string bootstrap_legality_semantics_replay_key;
  std::string bootstrap_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapFailureRestartSemanticsSummary(
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_semantics_contract_id.empty() &&
         !summary.bootstrap_reset_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.unsupported_topology_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.translation_unit_identity_key.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         !summary.replay_registered_images_symbol.empty() &&
         !summary.reset_replay_state_snapshot_symbol.empty() &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.semantic_boundary_ready &&
         summary.bootstrap_legality_semantics_contract_ready &&
         summary.bootstrap_semantics_contract_ready &&
         summary.bootstrap_reset_contract_ready &&
         summary.failure_mode_semantics_landed &&
         summary.restart_semantics_landed &&
         summary.replay_semantics_landed &&
         summary.unsupported_topology_semantics_landed &&
         summary.deterministic_recovery_semantics_landed &&
         summary.runtime_restart_probe_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.bootstrap_legality_semantics_replay_key.empty() &&
         !summary.bootstrap_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
