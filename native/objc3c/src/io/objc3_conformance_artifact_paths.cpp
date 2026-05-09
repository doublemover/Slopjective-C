#include "io/objc3_artifact_paths.h"

#include "lower/objc3_lowering_contract.h"

std::filesystem::path BuildVersionedConformanceReportArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir /
         (emit_prefix +
          kObjc3VersionedConformanceReportLoweringArtifactSuffix);
}

std::filesystem::path BuildConformancePublicationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-conformance-publication.json");
}

std::filesystem::path BuildConformanceValidationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-conformance-validation.json");
}

std::filesystem::path BuildReleaseEvidenceOperationArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-release-evidence-operation.json");
}

std::filesystem::path BuildDashboardStatusArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-dashboard-status.json");
}

std::filesystem::path BuildAdvancedFeatureGateArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-advanced-feature-gate.json");
}

std::filesystem::path BuildReleaseCandidateMatrixArtifactPath(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return out_dir / (emit_prefix + ".objc3-release-candidate-matrix.json");
}
