inline bool IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.publication_contract_id.empty() &&
         summary.matrix_published &&
         summary.fail_closed &&
         summary.uses_llvm_readobj &&
         summary.uses_llvm_objdump &&
         summary.matrix_row_count == 2u &&
         !summary.fixture_path.empty() &&
         !summary.emit_prefix.empty() &&
         !summary.object_relative_path.empty() &&
         !summary.section_inventory_row_key.empty() &&
         !summary.section_inventory_command.empty() &&
         !summary.symbol_inventory_row_key.empty() &&
         !summary.symbol_inventory_command.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSourceToSectionMatrixRow {
  std::string row_key;
  std::string graph_node_kind;
  std::string emission_mode;
  std::string logical_section;
  std::string payload_role;
  std::string descriptor_symbol_family;
  std::string aggregate_symbol;
  std::string relocation_behavior;
  std::string proof_fixture_path;
  std::string proof_mode;
  std::string section_inventory_command;
  std::string symbol_inventory_command;
};

struct Objc3RuntimeMetadataSourceToSectionMatrixSummary {
  std::string contract_id = kObjc3RuntimeMetadataSourceToSectionMatrixContractId;
  std::string source_graph_contract_id =
      kObjc3ExecutableMetadataSourceGraphContractId;
  std::string section_abi_contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  std::string section_publication_contract_id =
      kObjc3RuntimeMetadataSectionPublicationContractId;
  std::string object_inspection_contract_id =
      kObjc3RuntimeMetadataObjectInspectionContractId;
  std::string manifest_surface_path =
      kObjc3RuntimeMetadataSourceToSectionMatrixSurfacePath;
  std::string row_ordering_model =
      kObjc3RuntimeMetadataSourceToSectionMatrixOrderingModel;
  bool matrix_published = false;
  bool fail_closed = false;
  bool source_graph_ready = false;
  bool section_abi_ready = false;
  bool section_publication_ready = false;
  bool object_inspection_ready = false;
  bool supported_node_coverage_complete = false;
  bool explicit_non_goals_published = false;
  bool row_ordering_frozen = false;
  std::size_t matrix_row_count = 0;
  std::array<Objc3RuntimeMetadataSourceToSectionMatrixRow, 9u> rows = {};
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  if (summary.contract_id.empty() || summary.source_graph_contract_id.empty() ||
      summary.section_abi_contract_id.empty() ||
      summary.section_publication_contract_id.empty() ||
      summary.object_inspection_contract_id.empty() ||
      summary.manifest_surface_path.empty() ||
      summary.row_ordering_model.empty() || !summary.matrix_published ||
      !summary.fail_closed || !summary.source_graph_ready ||
      !summary.section_abi_ready || !summary.section_publication_ready ||
      !summary.object_inspection_ready ||
      !summary.supported_node_coverage_complete ||
      !summary.explicit_non_goals_published || !summary.row_ordering_frozen ||
      summary.matrix_row_count != summary.rows.size() || summary.replay_key.empty() ||
      !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &row : summary.rows) {
    if (row.row_key.empty() || row.graph_node_kind.empty() ||
        row.emission_mode.empty() || row.logical_section.empty() ||
        row.payload_role.empty() || row.descriptor_symbol_family.empty() ||
        row.aggregate_symbol.empty() || row.relocation_behavior.empty() ||
        row.proof_fixture_path.empty() || row.proof_mode.empty() ||
        row.section_inventory_command.empty() ||
        row.symbol_inventory_command.empty()) {
      return false;
    }
  }
  return true;
}

struct Objc3ExecutableMetadataDebugProjectionMatrixRow {
  std::string row_key;
  std::string artifact_kind;
  std::string fixture_path;
  std::string emit_prefix = kObjc3ExecutableMetadataDebugProjectionEmitPrefix;
  std::string artifact_relative_path;
  std::string probe_command;
  std::string inspection_command;
  std::string expected_anchor;
};

struct Objc3ExecutableMetadataDebugProjectionSummary {
  std::string contract_id = kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string source_graph_contract_id =
      kObjc3ExecutableMetadataSourceGraphContractId;
  std::string named_metadata_name =
      kObjc3ExecutableMetadataDebugProjectionNamedMetadataName;
  std::string manifest_surface_path =
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath;
  std::string typed_handoff_surface_path =
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath;
  std::string source_graph_surface_path =
      kObjc3ExecutableMetadataSourceGraphManifestSurfacePath;
  bool matrix_published = false;
  bool fail_closed = false;
  bool manifest_debug_surface_published = false;
  bool ir_named_metadata_published = false;
  bool replay_anchor_deterministic = false;
  bool active_typed_handoff_ready = false;
  std::size_t matrix_row_count = 0;
  std::array<Objc3ExecutableMetadataDebugProjectionMatrixRow, 3u> rows = {};
  std::string replay_key;
  std::string active_typed_handoff_replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  if (summary.contract_id.empty() || summary.typed_lowering_handoff_contract_id.empty() ||
      summary.source_graph_contract_id.empty() ||
      summary.named_metadata_name.empty() ||
      summary.manifest_surface_path.empty() ||
      summary.typed_handoff_surface_path.empty() ||
      summary.source_graph_surface_path.empty() || !summary.matrix_published ||
      !summary.fail_closed || !summary.manifest_debug_surface_published ||
      !summary.ir_named_metadata_published ||
      !summary.replay_anchor_deterministic || summary.matrix_row_count != summary.rows.size() ||
      summary.replay_key.empty() || !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &row : summary.rows) {
    if (row.row_key.empty() || row.artifact_kind.empty() || row.fixture_path.empty() ||
        row.emit_prefix.empty() || row.artifact_relative_path.empty() ||
        row.probe_command.empty() || row.inspection_command.empty() ||
        row.expected_anchor.empty()) {
      return false;
    }
  }
  return true;
}

struct Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary {
  std::string contract_id =
      kObjc3ExecutableMetadataRuntimeIngestPackagingContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string debug_projection_contract_id =
      kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string packaging_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath;
  std::string typed_handoff_surface_path =
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath;
  std::string debug_projection_surface_path =
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath;
  std::string packaging_payload_model =
      kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel;
  std::string transport_artifact_relative_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingTransportArtifact;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool typed_lowering_handoff_ready = false;
  bool debug_projection_ready = false;
  bool manifest_transport_frozen = false;
  bool runtime_section_emission_not_yet_landed = false;
  bool startup_registration_not_yet_landed = false;
  bool runtime_loader_registration_not_yet_landed = false;
  bool explicit_non_goals_published = false;
  bool ready_for_packaging_implementation = false;
  std::string typed_lowering_handoff_replay_key;
  std::string debug_projection_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.typed_lowering_handoff_contract_id.empty() &&
         !summary.debug_projection_contract_id.empty() &&
         !summary.packaging_surface_path.empty() &&
         !summary.typed_handoff_surface_path.empty() &&
         !summary.debug_projection_surface_path.empty() &&
         !summary.packaging_payload_model.empty() &&
         !summary.transport_artifact_relative_path.empty() &&
         summary.boundary_frozen && summary.fail_closed &&
         summary.typed_lowering_handoff_ready && summary.debug_projection_ready &&
         summary.manifest_transport_frozen &&
         summary.runtime_section_emission_not_yet_landed &&
         summary.startup_registration_not_yet_landed &&
         summary.runtime_loader_registration_not_yet_landed &&
         summary.explicit_non_goals_published &&
         summary.ready_for_packaging_implementation &&
         !summary.typed_lowering_handoff_replay_key.empty() &&
         !summary.debug_projection_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary {
  std::string contract_id =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId;
  std::string packaging_contract_id =
      kObjc3ExecutableMetadataRuntimeIngestPackagingContractId;
  std::string typed_lowering_handoff_contract_id =
      kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string debug_projection_contract_id =
      kObjc3ExecutableMetadataDebugProjectionContractId;
  std::string packaging_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath;
  std::string binary_boundary_surface_path =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath;
  std::string payload_model =
      kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel;
  std::string envelope_format =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat;
  std::string artifact_relative_path =
      kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath;
  std::string artifact_suffix =
      kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix;
  std::string binary_magic = kObjc3ExecutableMetadataRuntimeIngestBinaryMagic;
  std::uint32_t envelope_version =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion;
  std::uint32_t chunk_count =
      kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount;
  std::array<std::string, 3u> chunk_names = {
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName};
  bool fail_closed = false;
  bool packaging_contract_ready = false;
  bool typed_lowering_handoff_ready = false;
  bool debug_projection_ready = false;
  bool binary_payload_present = false;
  bool binary_boundary_emitted = false;
  bool binary_envelope_deterministic = false;
  bool ready_for_section_emission_handoff = false;
  std::size_t payload_bytes = 0;
  std::string packaging_contract_replay_key;
  std::string typed_lowering_handoff_replay_key;
  std::string debug_projection_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  if (summary.contract_id.empty() || summary.packaging_contract_id.empty() ||
      summary.typed_lowering_handoff_contract_id.empty() ||
      summary.debug_projection_contract_id.empty() ||
      summary.packaging_surface_path.empty() ||
      summary.binary_boundary_surface_path.empty() ||
      summary.payload_model.empty() || summary.envelope_format.empty() ||
      summary.artifact_relative_path.empty() || summary.artifact_suffix.empty() ||
      summary.binary_magic.empty() || summary.envelope_version == 0u ||
      summary.chunk_count != summary.chunk_names.size() || !summary.fail_closed ||
      !summary.packaging_contract_ready ||
      !summary.typed_lowering_handoff_ready ||
      !summary.debug_projection_ready || !summary.binary_payload_present ||
      !summary.binary_boundary_emitted ||
      !summary.binary_envelope_deterministic ||
      !summary.ready_for_section_emission_handoff || summary.payload_bytes == 0u ||
      summary.packaging_contract_replay_key.empty() ||
      summary.typed_lowering_handoff_replay_key.empty() ||
      summary.debug_projection_replay_key.empty() || summary.replay_key.empty() ||
      !summary.failure_reason.empty()) {
    return false;
  }
  for (const auto &chunk_name : summary.chunk_names) {
    if (chunk_name.empty()) {
      return false;
    }
  }
  return true;
}

struct Objc3RuntimeSupportLibraryContractSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryContractId;
  std::string metadata_publication_contract_id =
      kObjc3RuntimeMetadataSectionPublicationContractId;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool target_name_frozen = false;
  bool exported_entrypoints_frozen = false;
  bool ownership_boundaries_frozen = false;
  bool build_constraints_frozen = false;
  bool shim_remains_test_only = false;
  bool native_runtime_library_present = false;
  bool driver_link_wiring_pending = true;
  bool ready_for_runtime_library_skeleton = false;
  std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string source_root = kObjc3RuntimeSupportLibrarySourceRoot;
  std::string library_kind = kObjc3RuntimeSupportLibraryKind;
  std::string archive_basename = kObjc3RuntimeSupportLibraryArchiveBasename;
  std::string register_image_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string lookup_selector_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_i32_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryDriverLinkMode;
  std::string compiler_ownership_boundary =
      kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary;
  std::string runtime_ownership_boundary =
      kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryContractSummary(
    const Objc3RuntimeSupportLibraryContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.metadata_publication_contract_id.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.target_name_frozen &&
         summary.exported_entrypoints_frozen &&
         summary.ownership_boundaries_frozen &&
         summary.build_constraints_frozen &&
         summary.shim_remains_test_only &&
         !summary.native_runtime_library_present &&
         summary.driver_link_wiring_pending &&
         summary.ready_for_runtime_library_skeleton &&
         !summary.cmake_target_name.empty() &&
         !summary.public_header_path.empty() &&
         !summary.source_root.empty() &&
         !summary.library_kind.empty() &&
         !summary.archive_basename.empty() &&
         !summary.register_image_symbol.empty() &&
         !summary.lookup_selector_symbol.empty() &&
         !summary.dispatch_i32_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.driver_link_mode.empty() &&
         !summary.compiler_ownership_boundary.empty() &&
         !summary.runtime_ownership_boundary.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeSupportLibraryCoreFeatureSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  std::string support_library_contract_id = kObjc3RuntimeSupportLibraryContractId;
  std::string metadata_publication_contract_id =
      kObjc3RuntimeMetadataSectionPublicationContractId;
  bool fail_closed = false;
  bool native_runtime_library_sources_present = false;
  bool native_runtime_library_header_present = false;
  bool native_runtime_library_archive_build_enabled = false;
  bool native_runtime_library_entrypoints_implemented = false;
  bool selector_lookup_stateful = false;
  bool deterministic_dispatch_formula_matches_test_shim = false;
  bool reset_for_testing_supported = false;
  bool shim_remains_test_only = false;
  bool driver_link_wiring_pending = true;
  bool ready_for_driver_link_wiring = false;
  std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string source_root = kObjc3RuntimeSupportLibrarySourceRoot;
  std::string implementation_source_path =
      kObjc3RuntimeSupportLibraryImplementationSourcePath;
  std::string library_kind = kObjc3RuntimeSupportLibraryKind;
  std::string archive_basename = kObjc3RuntimeSupportLibraryArchiveBasename;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string probe_source_path = kObjc3RuntimeSupportLibraryProbeSourcePath;
  std::string register_image_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string lookup_selector_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_i32_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryDriverLinkMode;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_contract_id.empty() &&
         !summary.metadata_publication_contract_id.empty() &&
         summary.fail_closed &&
         summary.native_runtime_library_sources_present &&
         summary.native_runtime_library_header_present &&
         summary.native_runtime_library_archive_build_enabled &&
         summary.native_runtime_library_entrypoints_implemented &&
         summary.selector_lookup_stateful &&
         summary.deterministic_dispatch_formula_matches_test_shim &&
         summary.reset_for_testing_supported &&
         summary.shim_remains_test_only &&
         summary.driver_link_wiring_pending &&
         summary.ready_for_driver_link_wiring &&
         !summary.cmake_target_name.empty() &&
         !summary.public_header_path.empty() &&
         !summary.source_root.empty() &&
         !summary.implementation_source_path.empty() &&
         !summary.library_kind.empty() &&
         !summary.archive_basename.empty() &&
         !summary.archive_relative_path.empty() &&
         !summary.probe_source_path.empty() &&
         !summary.register_image_symbol.empty() &&
         !summary.lookup_selector_symbol.empty() &&
         !summary.dispatch_i32_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.driver_link_mode.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeSupportLibraryLinkWiringSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string support_library_core_feature_contract_id =
      kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  bool fail_closed = false;
  bool runtime_library_archive_available = false;
  bool compatibility_dispatch_alias_exported = false;
  bool driver_emits_runtime_link_contract = false;
  bool execution_smoke_consumes_runtime_library = false;
  bool shim_remains_test_only = false;
  bool ready_for_runtime_library_consumption = false;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string compatibility_dispatch_symbol =
      kObjc3RuntimeSupportLibraryCompatibilityDispatchSymbol;
  std::string runtime_dispatch_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string execution_smoke_script_path =
      kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryLinkWiringMode;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryLinkWiringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_core_feature_contract_id.empty() &&
         summary.fail_closed &&
         summary.runtime_library_archive_available &&
         summary.compatibility_dispatch_alias_exported &&
         summary.driver_emits_runtime_link_contract &&
         summary.execution_smoke_consumes_runtime_library &&
         summary.shim_remains_test_only &&
         summary.ready_for_runtime_library_consumption &&
         !summary.archive_relative_path.empty() &&
         !summary.compatibility_dispatch_symbol.empty() &&
         !summary.runtime_dispatch_symbol.empty() &&
         !summary.execution_smoke_script_path.empty() &&
         !summary.driver_link_mode.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeTranslationUnitRegistrationContractSummary {
  std::string contract_id =
      kObjc3RuntimeTranslationUnitRegistrationContractId;
  std::string binary_boundary_contract_id =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId;
  std::string archive_static_link_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  std::string object_emission_closeout_contract_id =
      kObjc3RuntimeMetadataObjectEmissionCloseoutContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string registration_surface_path =
      kObjc3RuntimeTranslationUnitRegistrationSurfacePath;
  std::string registration_payload_model =
      kObjc3RuntimeTranslationUnitRegistrationPayloadModel;
  std::array<std::string, 3u> runtime_owned_payload_artifacts = {
      kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath};
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_root_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel;
  std::string constructor_emission_mode =
      kObjc3RuntimeTranslationUnitRegistrationConstructorEmissionMode;
  std::string constructor_priority_policy =
      kObjc3RuntimeTranslationUnitRegistrationConstructorPriorityPolicy;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool binary_boundary_ready = false;
  bool archive_static_link_surface_ready = false;
  bool object_emission_closeout_surface_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool runtime_owned_payload_inventory_published = false;
  bool constructor_root_reserved_not_emitted = false;
  bool startup_registration_not_yet_landed = false;
  bool runtime_bootstrap_not_yet_landed = false;
  bool explicit_non_goals_published = false;
  bool ready_for_registration_manifest_implementation = false;
  std::size_t runtime_owned_payload_artifact_count = 0;
  std::string binary_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.binary_boundary_contract_id.empty() &&
         !summary.archive_static_link_contract_id.empty() &&
         !summary.object_emission_closeout_contract_id.empty() &&
         !summary.runtime_support_library_link_wiring_contract_id.empty() &&
         !summary.registration_surface_path.empty() &&
         !summary.registration_payload_model.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.binary_boundary_ready &&
         summary.archive_static_link_surface_ready &&
         summary.object_emission_closeout_surface_ready &&
         summary.runtime_support_library_link_wiring_ready &&
         summary.runtime_owned_payload_inventory_published &&
         summary.constructor_root_reserved_not_emitted &&
         summary.startup_registration_not_yet_landed &&
         summary.runtime_bootstrap_not_yet_landed &&
         summary.explicit_non_goals_published &&
         summary.ready_for_registration_manifest_implementation &&
         summary.runtime_owned_payload_artifact_count ==
             summary.runtime_owned_payload_artifacts.size() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_root_ownership_model.empty() &&
         !summary.constructor_emission_mode.empty() &&
         !summary.constructor_priority_policy.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.binary_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

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

struct Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary {
  std::string contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3CompatibilityStrictnessClaimSemanticModel;
  std::string downgrade_model =
      kObjc3CompatibilityStrictnessClaimDowngradeModel;
  std::string rejection_model =
      kObjc3CompatibilityStrictnessClaimRejectionModel;
  std::string canonical_interface_truth_model =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel;
  std::string separate_compilation_macro_truth_model =
      kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel;
  std::string canonical_interface_payload_mode =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode;
  std::string effective_language_profile = "canonical";
  std::vector<std::string> suppressed_macro_claim_ids;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::size_t valid_language_profile_count = 0;
  std::size_t live_selection_surface_count = 0;
  std::size_t valid_selection_combination_count = 0;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t downgraded_source_only_claim_count = 0;
  std::size_t rejected_unsupported_feature_claim_count = 0;
  std::size_t rejected_selection_surface_count = 0;
  std::size_t suppressed_macro_claim_count = 0;
  // unsupported-feature enforcement anchor: the frontend
  // mirror of the sema packet must preserve zero-site runnable proofs and
  // deterministic early rejection accounting for accepted advanced surfaces.
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool language_profile_semantics_landed = false;
  bool canonical_literal_rejection_semantics_landed = false;
  bool source_only_claim_downgrade_semantics_landed = false;
  bool unsupported_feature_claim_rejection_semantics_landed = false;
  bool live_unsupported_feature_source_rejection_landed = false;
  bool strictness_selection_rejection_semantics_landed = false;
  bool feature_macro_claim_suppression_semantics_landed = false;
  bool canonical_interface_truth_semantics_landed = false;
  bool separate_compilation_macro_truth_semantics_landed = false;
  bool selected_configuration_valid = false;
  bool selected_configuration_downgraded = false;
