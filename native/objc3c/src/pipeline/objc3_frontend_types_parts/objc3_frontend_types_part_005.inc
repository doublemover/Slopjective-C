  bool selected_configuration_rejected = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.semantic_model.empty() && !summary.downgrade_model.empty() &&
         !summary.rejection_model.empty() &&
         !summary.canonical_interface_truth_model.empty() &&
         !summary.separate_compilation_macro_truth_model.empty() &&
         summary.canonical_interface_payload_mode ==
             kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode &&
         language_profile_valid && summary.fail_closed && summary.semantic_boundary_ready &&
         summary.suppressed_macro_claim_ids.size() ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.suppressed_macro_claim_ids[0] ==
             kObjc3SuppressedMacroClaimStrictnessLevel &&
         summary.suppressed_macro_claim_ids[1] ==
             kObjc3SuppressedMacroClaimConcurrencyMode &&
         summary.suppressed_macro_claim_ids[2] ==
             kObjc3SuppressedMacroClaimConcurrencyStrict &&
         summary.valid_language_profile_count ==
             kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount &&
         summary.live_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount &&
         summary.valid_selection_combination_count ==
             kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount &&
         summary.runnable_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRunnableFeatureCount &&
         summary.downgraded_source_only_claim_count ==
             kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount &&
         summary.rejected_unsupported_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRejectedFeatureCount &&
         summary.live_unsupported_feature_family_count <=
             summary.rejected_unsupported_feature_claim_count &&
         summary.throws_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.blocks_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.arc_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.throws_source_rejection_site_count +
                 summary.blocks_source_rejection_site_count +
                 summary.arc_source_rejection_site_count ==
             summary.live_unsupported_feature_site_count &&
         summary.rejected_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount &&
         summary.suppressed_macro_claim_count ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.language_profile_semantics_landed &&
         summary.canonical_literal_rejection_semantics_landed &&
         summary.source_only_claim_downgrade_semantics_landed &&
         summary.unsupported_feature_claim_rejection_semantics_landed &&
         summary.live_unsupported_feature_source_rejection_landed &&
         summary.strictness_selection_rejection_semantics_landed &&
         summary.feature_macro_claim_suppression_semantics_landed &&
         summary.canonical_interface_truth_semantics_landed &&
         summary.separate_compilation_macro_truth_semantics_landed &&
         summary.selected_configuration_valid &&
         !summary.selected_configuration_downgraded &&
         !summary.selected_configuration_rejected &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendTypeSystemTypeSourceClosureSummary(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3TypeSystemTypeSourceClosureContractId &&
         summary.frontend_surface_path == kObjc3TypeSystemTypeSourceClosureSurfacePath &&
         summary.source_model == kObjc3TypeSystemTypeSourceClosureSourceModel &&
         summary.failure_model == kObjc3TypeSystemTypeSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 7 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes &&
         summary.source_only_claim_ids[3] ==
             kObjc3SourceOnlyFeatureClaimOptionalBindings &&
         summary.source_only_claim_ids[4] ==
             kObjc3SourceOnlyFeatureClaimOptionalSends &&
         summary.source_only_claim_ids[5] ==
             kObjc3SourceOnlyFeatureClaimNilCoalescing &&
         summary.source_only_claim_ids[6] ==
             kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals &&
         summary.unsupported_claim_ids.empty() &&
         summary.protocol_optional_partition_source_supported &&
         summary.object_pointer_nullability_source_supported &&
         summary.pragmatic_generic_suffix_source_supported &&
         summary.optional_binding_source_supported &&
         summary.optional_send_source_supported &&
         summary.nil_coalescing_source_supported &&
         summary.typed_keypath_literal_source_supported &&
         !summary.optional_member_access_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendControlFlowControlFlowSourceClosureSummary(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3ControlFlowControlFlowSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3ControlFlowControlFlowSourceClosureSurfacePath &&
         summary.source_model == kObjc3ControlFlowControlFlowSourceClosureSourceModel &&
         summary.failure_model ==
             kObjc3ControlFlowControlFlowSourceClosureFailureModel &&
         summary.supported_construct_ids.size() == 8 &&
         summary.supported_construct_ids[0] ==
             kObjc3ControlFlowSourceSurfaceGuardBindings &&
         summary.supported_construct_ids[1] ==
             kObjc3ControlFlowSourceSurfaceGuardConditionLists &&
         summary.supported_construct_ids[2] ==
             kObjc3ControlFlowSourceSurfaceSwitchCasePatterns &&
         summary.supported_construct_ids[3] ==
             kObjc3ControlFlowSourceSurfaceDeferStatements &&
         summary.supported_construct_ids[4] ==
             kObjc3ControlFlowSourceSurfaceMatchStatement &&
         summary.supported_construct_ids[5] ==
             kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns &&
         summary.supported_construct_ids[6] ==
             kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns &&
         summary.supported_construct_ids[7] ==
             kObjc3ControlFlowSourceSurfaceMatchBindingPatterns &&
         summary.supported_construct_ids[8] ==
             kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ControlFlowFailClosedConstructMatchExpression &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ControlFlowFailClosedConstructGuardedPatterns &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns &&
         summary.guard_binding_source_supported &&
         summary.guard_condition_list_source_supported &&
         summary.switch_case_pattern_source_supported &&
         summary.defer_statement_source_supported &&
         summary.match_statement_source_supported &&
         summary.match_wildcard_pattern_source_supported &&
         summary.match_literal_pattern_source_supported &&
         summary.match_binding_pattern_source_supported &&
         summary.match_result_case_pattern_source_supported &&
         summary.defer_keyword_reserved && !summary.defer_fail_closed &&
         summary.match_expression_fail_closed &&
         summary.guarded_pattern_fail_closed &&
         summary.type_test_pattern_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendErrorHandlingErrorSourceClosureSummary(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3ErrorHandlingErrorSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3ErrorHandlingErrorSourceClosureSurfacePath &&
         summary.source_model == kObjc3ErrorHandlingErrorSourceClosureSourceModel &&
         summary.failure_model == kObjc3ErrorHandlingErrorSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 3 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimThrowsDeclarations &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimResultCarrierProfiles &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ErrorHandlingFailClosedConstructTryExpressions &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ErrorHandlingFailClosedConstructThrowStatements &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ErrorHandlingFailClosedConstructDoCatchStatements &&
         summary.throws_declaration_source_supported &&
         summary.result_carrier_source_supported &&
         summary.ns_error_bridging_source_supported &&
         summary.error_bridge_marker_source_supported &&
         summary.try_keyword_reserved && summary.throw_keyword_reserved &&
         summary.catch_keyword_reserved && summary.try_fail_closed &&
         summary.throw_fail_closed && summary.do_catch_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3VersionedConformanceReportLoweringSummary {
  std::string contract_id =
      kObjc3VersionedConformanceReportLoweringContractId;
  std::string semantic_contract_id =
      kObjc3VersionedConformanceReportLoweringSemanticContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3VersionedConformanceReportLoweringSurfacePath;
  std::string artifact_suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  std::string artifact_schema_id =
      kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  std::string payload_model =
      kObjc3VersionedConformanceReportLoweringPayloadModel;
  std::string authority_model =
      kObjc3VersionedConformanceReportLoweringAuthorityModel;
  std::string known_unsupported_model =
      kObjc3VersionedConformanceReportKnownUnsupportedModel;
  std::string selection_model =
      kObjc3VersionedConformanceReportSelectionModel;
  std::string canonical_interface_mode =
      kObjc3VersionedConformanceReportCanonicalInterfaceMode;
  std::string publication_model =
      kObjc3VersionedConformanceReportPublicationModel;
  std::string effective_language_profile = "canonical";
  std::vector<std::string> runnable_feature_claim_ids;
  std::vector<std::string> source_only_feature_claim_ids;
  std::vector<std::string> unsupported_feature_claim_ids;
  std::vector<std::string> suppressed_macro_claim_ids;
  bool fail_closed = false;
  bool semantic_surface_published = false;
  bool runnable_claim_inventory_ready = false;
  bool feature_claim_truth_surface_ready = false;
  bool semantic_boundary_ready = false;
  bool known_unsupported_surface_published = false;
  bool compatibility_selection_truthful = false;
  bool strictness_selection_fail_closed = false;
  bool strict_concurrency_selection_fail_closed = false;
  bool canonical_interface_truthful = false;
  bool feature_macro_truthful = false;
  bool ready_for_runtime_conformance_publication = false;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t source_only_feature_claim_count = 0;
  std::size_t unsupported_feature_claim_count = 0;
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::string runnable_feature_claim_inventory_replay_key;
  std::string feature_claim_truth_surface_replay_key;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingMachineReadableConformanceReportContractSummary {
  std::string contract_id =
      kObjc3ToolingMachineReadableConformanceReportContractId;
  std::string dependency_contract_id =
      kObjc3ToolingMachineReadableConformanceReportDependencyContractId;
  std::string lowering_contract_id =
      kObjc3VersionedConformanceReportLoweringContractId;
  std::string runtime_capability_contract_id =
      kObjc3RuntimeCapabilityReportingContractId;
  std::string frontend_surface_path =
      kObjc3ToolingMachineReadableConformanceReportSurfacePath;
  std::string payload_model =
      kObjc3ToolingMachineReadableConformanceReportPayloadModel;
  std::string authority_model =
      kObjc3ToolingMachineReadableConformanceReportAuthorityModel;
  std::string artifact_suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  std::string artifact_schema_id =
      kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  std::string runtime_capability_schema_id =
      kObjc3RuntimeCapabilityReportingSchemaId;
  std::string effective_language_profile = "canonical";
  bool migration_semantics_ready = false;
  bool lowering_contract_ready = false;
  bool runtime_capability_surface_published = false;
  bool deterministic_handoff = false;
  bool ready_for_runtime_publication = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingFeatureAwareConformanceReportEmissionSummary {
  std::string contract_id =
      kObjc3ToolingFeatureAwareConformanceReportEmissionContractId;
  std::string dependency_contract_id =
      kObjc3ToolingFeatureAwareConformanceReportEmissionDependencyContractId;
  std::string machine_readable_report_contract_id =
      kObjc3ToolingMachineReadableConformanceReportContractId;
  std::string fixit_contract_id =
      kObjc3ToolingFeatureSpecificFixitSynthesisContractId;
  std::string migration_semantics_contract_id =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3ToolingFeatureAwareConformanceReportEmissionSurfacePath;
  std::string payload_model =
      kObjc3ToolingFeatureAwareConformanceReportEmissionPayloadModel;
  std::string authority_model =
      kObjc3ToolingFeatureAwareConformanceReportEmissionAuthorityModel;
  std::string effective_language_profile = "canonical";
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::vector<std::string> fixit_family_ids;
  std::size_t fixit_family_count = 0;
  std::size_t current_run_legacy_literal_sites = 0;
  std::string canonical_mode_rejection_code =
      kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode;
  bool report_payload_emitted = false;
  bool deterministic_handoff = false;
  bool ready_for_runtime_publication = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary {
  std::string contract_id =
      kObjc3ToolingCorpusShardingReleaseEvidencePackagingContractId;
  std::string dependency_contract_id =
      kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId;
  std::string feature_aware_report_contract_id =
      kObjc3ToolingFeatureAwareConformanceReportEmissionContractId;
  std::string machine_readable_report_contract_id =
      kObjc3ToolingMachineReadableConformanceReportContractId;
  std::string frontend_surface_path =
      kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath;
  std::string payload_model =
      kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel;
  std::string authority_model =
      kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel;
  std::vector<std::string> targeted_profile_ids = {
      "strict", "strict-concurrency", "strict-system"};
  std::size_t targeted_profile_count = 3;
  std::vector<std::string> corpus_shard_ids = {
      "parser", "semantic", "lowering_abi", "module_roundtrip",
      "diagnostics"};
  std::vector<std::string> corpus_shard_manifest_paths = {
      "tests/conformance/parser/manifest.json",
      "tests/conformance/semantic/manifest.json",
      "tests/conformance/lowering_abi/manifest.json",
      "tests/conformance/module_roundtrip/manifest.json",
      "tests/conformance/diagnostics/manifest.json"};
  std::size_t corpus_shard_count = 5;
  std::vector<std::string> release_evidence_artifact_ids = {
      "EVID-01", "EVID-02", "EVID-03", "EVID-04", "EVID-07",
      "EVID-08", "EVID-09", "EVID-10", "EVID-11"};
  std::size_t release_evidence_artifact_count = 9;
  std::string release_evidence_checklist_path =
      "spec/conformance/profile_release_evidence_checklist.md";
  std::string release_evidence_schema_path =
      "spec/conformance/objc3_conformance_evidence_bundle_schema.md";
  std::string report_artifact_suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  std::string effective_language_profile = "canonical";
  bool canonical_literal_rejection_diagnostics_enabled = false;
  bool feature_report_payload_emitted = false;
  bool deterministic_handoff = false;
  bool ready_for_release_evidence_packaging = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3VersionedConformanceReportLoweringSummary(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.semantic_contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.artifact_suffix.empty() &&
         !summary.artifact_schema_id.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() &&
         !summary.known_unsupported_model.empty() &&
         !summary.selection_model.empty() &&
         !summary.canonical_interface_mode.empty() &&
         !summary.publication_model.empty() && language_profile_valid &&
         summary.runnable_feature_claim_count ==
             summary.runnable_feature_claim_ids.size() &&
         summary.source_only_feature_claim_count ==
             summary.source_only_feature_claim_ids.size() &&
         summary.unsupported_feature_claim_count ==
             summary.unsupported_feature_claim_ids.size() &&
         summary.live_unsupported_feature_family_count <=
             summary.unsupported_feature_claim_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.fail_closed && summary.semantic_surface_published &&
         summary.runnable_claim_inventory_ready &&
         summary.feature_claim_truth_surface_ready &&
         summary.semantic_boundary_ready &&
         summary.known_unsupported_surface_published &&
         summary.compatibility_selection_truthful &&
         summary.strictness_selection_fail_closed &&
         summary.strict_concurrency_selection_fail_closed &&
         summary.canonical_interface_truthful &&
         summary.feature_macro_truthful &&
         summary.ready_for_runtime_conformance_publication &&
         !summary.runnable_feature_claim_inventory_replay_key.empty() &&
         !summary.feature_claim_truth_surface_replay_key.empty() &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3ToolingMachineReadableConformanceReportContractSummary(
    const Objc3ToolingMachineReadableConformanceReportContractSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.lowering_contract_id.empty() &&
         !summary.runtime_capability_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() &&
         !summary.artifact_suffix.empty() &&
         !summary.artifact_schema_id.empty() &&
         !summary.runtime_capability_schema_id.empty() &&
         language_profile_valid && summary.migration_semantics_ready &&
         summary.lowering_contract_ready &&
         summary.runtime_capability_surface_published &&
         summary.deterministic_handoff &&
         summary.ready_for_runtime_publication && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline bool IsReadyObjc3ToolingFeatureAwareConformanceReportEmissionSummary(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.machine_readable_report_contract_id.empty() &&
         !summary.fixit_contract_id.empty() &&
         !summary.migration_semantics_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() && language_profile_valid &&
         summary.fixit_family_count == summary.fixit_family_ids.size() &&
         !summary.canonical_mode_rejection_code.empty() &&
         summary.report_payload_emitted && summary.deterministic_handoff &&
         summary.ready_for_runtime_publication && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline bool IsReadyObjc3ToolingCorpusShardingReleaseEvidencePackagingSummary(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.feature_aware_report_contract_id.empty() &&
         !summary.machine_readable_report_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() && language_profile_valid &&
         summary.targeted_profile_count == summary.targeted_profile_ids.size() &&
         summary.corpus_shard_count == summary.corpus_shard_ids.size() &&
         summary.corpus_shard_count ==
             summary.corpus_shard_manifest_paths.size() &&
         summary.release_evidence_artifact_count ==
             summary.release_evidence_artifact_ids.size() &&
         !summary.release_evidence_checklist_path.empty() &&
         !summary.release_evidence_schema_path.empty() &&
         !summary.report_artifact_suffix.empty() &&
         summary.feature_report_payload_emitted &&
         summary.deterministic_handoff &&
         summary.ready_for_release_evidence_packaging &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeBootstrapApiSummary {
  std::string contract_id = kObjc3RuntimeBootstrapApiContractId;
  std::string support_library_core_feature_contract_id =
      kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  std::string support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string bootstrap_surface_path = kObjc3RuntimeBootstrapApiSurfacePath;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string registration_status_enum_type =
      kObjc3RuntimeBootstrapApiStatusEnumType;
  std::string image_descriptor_type =
      kObjc3RuntimeBootstrapApiImageDescriptorType;
  std::string selector_handle_type =
      kObjc3RuntimeBootstrapApiSelectorHandleType;
  std::string registration_snapshot_type =
      kObjc3RuntimeBootstrapApiRegistrationSnapshotType;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_entrypoint_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string state_snapshot_symbol = kObjc3RuntimeBootstrapStateSnapshotSymbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string compatibility_dispatch_symbol =
      kObjc3RuntimeSupportLibraryCompatibilityDispatchSymbol;
  std::string registration_result_model = kObjc3RuntimeBootstrapResultModel;
  std::string registration_order_ordinal_model =
      kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
  std::string runtime_state_locking_model =
      kObjc3RuntimeBootstrapApiStateLockingModel;
  std::string startup_invocation_model =
      kObjc3RuntimeBootstrapApiStartupInvocationModel;
  std::string image_walk_lifecycle_model =
      kObjc3RuntimeBootstrapApiImageWalkLifecycleModel;
  std::string deterministic_reset_lifecycle_model =
      kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel;
  bool fail_closed = false;
  bool support_library_core_feature_contract_ready = false;
  bool support_library_link_wiring_contract_ready = false;
  bool api_surface_frozen = false;
  bool registration_entrypoint_frozen = false;
  bool selector_lookup_and_dispatch_frozen = false;
  bool reset_and_snapshot_hooks_frozen = false;
  bool runtime_probe_required = false;
  bool image_walk_not_yet_landed = false;
  bool deterministic_reset_expansion_not_yet_landed = false;
  bool ready_for_registrar_implementation = false;
  std::string support_library_core_feature_replay_key;
  std::string support_library_link_wiring_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeBootstrapApiSummary(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_core_feature_contract_id.empty() &&
         !summary.support_library_link_wiring_contract_id.empty() &&
         !summary.bootstrap_surface_path.empty() &&
         !summary.public_header_path.empty() &&
         !summary.archive_relative_path.empty() &&
         !summary.registration_status_enum_type.empty() &&
         !summary.image_descriptor_type.empty() &&
         !summary.selector_handle_type.empty() &&
         !summary.registration_snapshot_type.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.selector_lookup_symbol.empty() &&
         !summary.dispatch_entrypoint_symbol.empty() &&
         !summary.state_snapshot_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.compatibility_dispatch_symbol.empty() &&
         !summary.registration_result_model.empty() &&
         !summary.registration_order_ordinal_model.empty() &&
         !summary.runtime_state_locking_model.empty() &&
         !summary.startup_invocation_model.empty() &&
         !summary.image_walk_lifecycle_model.empty() &&
         !summary.deterministic_reset_lifecycle_model.empty() &&
         summary.fail_closed &&
         summary.support_library_core_feature_contract_ready &&
         summary.support_library_link_wiring_contract_ready &&
         summary.api_surface_frozen &&
         summary.registration_entrypoint_frozen &&
         summary.selector_lookup_and_dispatch_frozen &&
         summary.reset_and_snapshot_hooks_frozen &&
         summary.runtime_probe_required &&
         summary.image_walk_not_yet_landed &&
         summary.deterministic_reset_expansion_not_yet_landed &&
         summary.ready_for_registrar_implementation &&
         !summary.support_library_core_feature_replay_key.empty() &&
         !summary.support_library_link_wiring_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3FrontendPipelineResult {
  Objc3ParsedProgram program;
  Objc3ParserContractSnapshot parser_contract_snapshot;
  Objc3FrontendDiagnosticsBus stage_diagnostics;
  Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold
      semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
      semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface
      semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface
      semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface
      semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface
      semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface
      semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface
      semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface
      semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface;
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface
      semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface;
  Objc3ParseLoweringReadinessSurface parse_lowering_readiness_surface;
  Objc3SemanticStabilitySpecDeltaClosureScaffold semantic_stability_spec_delta_closure_scaffold;
  Objc3SemanticStabilityCoreFeatureImplementationSurface
      semantic_stability_core_feature_implementation_surface;
  Objc3LoweringRuntimeStabilityInvariantScaffold lowering_runtime_stability_invariant_scaffold;
  Objc3LoweringPipelinePassGraphScaffold lowering_pipeline_pass_graph_scaffold;
  Objc3LoweringPipelinePassGraphCoreFeatureSurface lowering_pipeline_pass_graph_core_feature_surface;
  Objc3IREmissionCompletenessScaffold ir_emission_completeness_scaffold;
  Objc3LoweringRuntimeDiagnosticsSurfacingScaffold
      lowering_runtime_diagnostics_surfacing_scaffold;
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
      lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface
      lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
      lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
      lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
      lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface;
  Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
      lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface;
  Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface
      lowering_runtime_stability_core_feature_implementation_surface;
  Objc3FrontendMigrationHints migration_hints;
  Objc3FrontendLanguageVersionPragmaContract language_version_pragma_contract;
  Objc3FrontendBootstrapRegistrationSourcePragmaContract
      bootstrap_registration_source_pragma_contract;
  Objc3SemanticIntegrationSurface integration_surface;
  Objc3SemanticTypeMetadataHandoff sema_type_metadata_handoff;
  Objc3FrontendProtocolCategorySummary protocol_category_summary;
  Objc3FrontendClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3FrontendSelectorNormalizationSummary selector_normalization_summary;
  Objc3FrontendPropertyAttributeSummary property_attribute_summary;
  Objc3FrontendObjectPointerNullabilityGenericsSummary object_pointer_nullability_generics_summary;
  Objc3FrontendTypeSystemTypeSourceClosureSummary type_system_type_source_closure_summary;
  Objc3FrontendControlFlowControlFlowSourceClosureSummary
      control_flow_control_flow_source_closure_summary;
  Objc3FrontendErrorHandlingErrorSourceClosureSummary error_handling_error_source_closure_summary;
  Objc3FrontendConcurrencyAsyncSourceClosureSummary concurrency_async_source_closure_summary;
  Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
      ownership_system_extension_source_closure_summary;
  Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
      ownership_cleanup_resource_capture_source_completion_summary;
  Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
      ownership_retainable_c_family_source_completion_summary;
  Objc3FrontendDispatchDispatchIntentSourceClosureSummary
      dispatch_dispatch_intent_source_closure_summary;
  Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
      dispatch_dispatch_intent_source_completion_summary;
  Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
      metaprogramming_metaprogramming_source_closure_summary;
  Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
      metaprogramming_macro_package_provenance_source_completion_summary;
  Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
      metaprogramming_property_behavior_source_completion_summary;
  Objc3FrontendInteropForeignImportSourceClosureSummary
      interop_foreign_import_source_closure_summary;
  Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
      interop_cpp_swift_interop_annotation_source_completion_summary;
  Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
      tooling_diagnostics_migrator_source_inventory_summary;
  Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
      tooling_migration_canonicalization_source_completion_summary;
  Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
      tooling_diagnostic_taxonomy_portability_contract_summary;
  Objc3ToolingFeatureSpecificFixitSynthesisSummary
      tooling_feature_specific_fixit_synthesis_summary;
  Objc3InteropForeignSurfaceInterfacePreservationSummary
      interop_foreign_surface_interface_preservation_summary;
  Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
      concurrency_actor_member_isolation_source_closure_summary;
  Objc3ConcurrencyActorIsolationSendableSemanticModelSummary
      concurrency_actor_isolation_sendable_semantic_model_summary;
  Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
      concurrency_actor_isolation_sendability_enforcement_summary;
  Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary
      concurrency_actor_race_hazard_escape_diagnostics_summary;
  Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
      concurrency_task_group_cancellation_source_closure_summary;
  Objc3ControlFlowControlFlowSemanticModelSummary
      control_flow_control_flow_semantic_model_summary;
  Objc3ErrorHandlingErrorSemanticModelSummary error_handling_error_semantic_model_summary;
  Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
      concurrency_task_executor_cancellation_semantic_model_summary;
  Objc3OwnershipSystemExtensionSemanticModelSummary
      ownership_system_extension_semantic_model_summary;
  Objc3EffectsOwnershipSemanticModelSummary
      effects_ownership_semantic_model_summary;
  Objc3CrossModuleSemanticContractsDiagnosticsSummary
      cross_module_semantic_contracts_diagnostics_summary;
  Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary
      metaprogramming_expansion_behavior_semantic_model_summary;
  Objc3MetaprogrammingDeriveExpansionInventorySummary
      metaprogramming_derive_expansion_inventory_summary;
  Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary
      metaprogramming_macro_safety_sandbox_determinism_summary;
  Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
      metaprogramming_property_behavior_legality_compatibility_summary;
    Objc3DispatchDispatchIntentSemanticModelSummary
        dispatch_dispatch_intent_semantic_model_summary;
    Objc3DispatchDispatchIntentLegalitySummary
        dispatch_dispatch_intent_legality_summary;
    Objc3DispatchDispatchIntentCompatibilitySummary
        dispatch_dispatch_intent_compatibility_summary;
    Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        ownership_resource_move_use_after_move_semantics_summary;
  Objc3OwnershipBorrowedPointerEscapeAnalysisSummary
      ownership_borrowed_pointer_escape_analysis_summary;
  Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
      ownership_capture_list_retainable_family_legality_completion_summary;
  Objc3InteropInteropSemanticModelSummary
      interop_interop_semantic_model_summary;
  Objc3InteropInteropRuntimeParitySummary
      interop_interop_runtime_parity_summary;
  Objc3InteropCppInteropInteractionSummary
      interop_cpp_interop_interaction_summary;
  Objc3InteropSwiftInteropIsolationSummary
      interop_swift_interop_isolation_summary;
  Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
      concurrency_structured_task_cancellation_semantic_summary;
  Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
      concurrency_executor_hop_affinity_compatibility_summary;
  Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary
      concurrency_async_effect_suspension_semantic_model_summary;
  Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
      concurrency_await_suspension_resume_semantic_summary;
  Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
      concurrency_async_diagnostics_compatibility_summary;
  Objc3ErrorHandlingTryDoCatchSemanticSummary error_handling_try_do_catch_semantic_summary;
  Objc3ErrorHandlingErrorBridgeLegalitySummary error_handling_error_bridge_legality_summary;
  Objc3FrontendSymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3RuntimeMetadataSourceRecordSet runtime_metadata_source_records;
  Objc3ExecutableMetadataSourceGraph executable_metadata_source_graph;
  Objc3ExecutableMetadataSemanticConsistencyBoundary
      executable_metadata_semantic_consistency_boundary;
  Objc3ExecutableMetadataSemanticValidationSurface
      executable_metadata_semantic_validation_surface;
  Objc3ExecutableMetadataLoweringHandoffSurface
      executable_metadata_lowering_handoff_surface;
  Objc3ExecutableMetadataTypedLoweringHandoff
      executable_metadata_typed_lowering_handoff;
  Objc3RuntimeMetadataSourceOwnershipBoundary runtime_metadata_source_ownership_boundary;
  Objc3RuntimeExportLegalityBoundary runtime_export_legality_boundary;
  Objc3RuntimeExportEnforcementSummary runtime_export_enforcement_summary;
  std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};
  Objc3SemaPassFlowSummary sema_pass_flow_summary;
  Objc3SemaParityContractSurface sema_parity_surface;
};
