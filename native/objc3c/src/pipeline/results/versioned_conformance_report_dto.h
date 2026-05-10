#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lower/contracts/conformance_versioned_report_contracts.h"
#include "token/objc3_token_contract.h"

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
