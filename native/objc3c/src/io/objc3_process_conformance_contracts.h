#pragma once

#include <filesystem>
#include <string>
#include <vector>

inline constexpr const char *kObjc3ReleaseEvidenceGatePublicCommand =
    "npm run objc3c -- check-release-evidence";
inline constexpr const char *kObjc3ReleaseEvidenceGateScriptPath =
    "scripts/check_release_evidence.py";

struct Objc3ConformanceReportPublicationArtifactInputs {
  std::string contract_id;
  std::string schema_id;
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::string effective_language_profile;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::string publication_model;
  std::string publication_surface_kind;
  std::string fail_closed_diagnostic_model;
  std::string lowered_report_contract_id;
  std::string runtime_capability_contract_id;
  std::string public_conformance_schema_id;
  std::string advanced_feature_ops_contract_id;
  std::string advanced_feature_reporting_contract_id;
  std::string advanced_feature_release_evidence_contract_id;
  std::string ci_release_evidence_gate_script_path;
  std::string runbook_reference_path;
  std::string dashboard_schema_path;
  std::vector<std::string> advanced_feature_targeted_profile_ids;
  std::string report_artifact_relative_path;
};

struct Objc3ConformanceClaimValidationArtifactInputs {
  std::string report_artifact_path;
  std::string publication_artifact_path;
};

struct Objc3ReleaseEvidenceOperationArtifactInputs {
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string dashboard_artifact_path;
};

struct Objc3DashboardStatusArtifactInputs {
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
};

struct Objc3AdvancedFeatureGateArtifactInputs {
  std::string surface_kind;
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
  std::string dashboard_artifact_path;
};

struct Objc3ReleaseCandidateMatrixArtifactInputs {
  std::string surface_kind;
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string advanced_feature_gate_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
  std::string dashboard_artifact_path;
};

std::vector<std::string> BuildObjc3ClaimedConformanceProfileIds();
std::vector<std::string> BuildObjc3RejectedConformanceProfileIds();
std::vector<std::string> BuildObjc3ReleaseTargetedProfileIds();

std::vector<std::filesystem::path>
BuildObjc3RetiredClaimSidecarPaths(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix);

bool DiagnoseObjc3RetiredClaimSidecars(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    std::string &error);

bool IsObjc3ClaimedConformanceProfile(const std::string &profile_id);
bool IsObjc3JsonConformanceFormat(const std::string &format);

std::string BuildUnsupportedObjc3ConformanceProfileSelectionDiagnostic(
    const std::string &profile_id);

std::string BuildUnsupportedObjc3ConformanceFormatSelectionDiagnostic(
    const std::string &format);

bool TryBuildObjc3ReleaseEvidenceOperationArtifact(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    std::string &artifact_json,
    std::string &error);

bool TryBuildObjc3DashboardStatusArtifact(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json,
    std::string &artifact_json,
    std::string &error);

bool TryBuildObjc3AdvancedFeatureGateArtifact(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error);

bool TryBuildObjc3ReleaseCandidateMatrixArtifact(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &advanced_feature_gate_json,
    std::string &artifact_json,
    std::string &error);

bool TryBuildObjc3ConformanceReportPublicationArtifact(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs,
    std::string &artifact_json,
    std::string &error);

bool TryBuildObjc3ConformanceClaimValidationArtifact(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error);
