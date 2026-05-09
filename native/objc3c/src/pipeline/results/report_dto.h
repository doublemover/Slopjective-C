#pragma once

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
  std::size_t current_run_canonical_literal_rejection_sites = 0;
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
