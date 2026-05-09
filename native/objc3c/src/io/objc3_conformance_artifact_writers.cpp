#include "io/objc3_artifact_writers.h"

#include "io/objc3_artifact_paths.h"
#include "io/objc3_file_io.h"

void WriteVersionedConformanceReportArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildVersionedConformanceReportArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteConformancePublicationArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildConformancePublicationArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteConformanceValidationArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildConformanceValidationArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteReleaseEvidenceOperationArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildReleaseEvidenceOperationArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteDashboardStatusArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
                                  const std::string &artifact_json) {
  WriteText(BuildDashboardStatusArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteAdvancedFeatureGateArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildAdvancedFeatureGateArtifactPath(out_dir, emit_prefix),
            artifact_json);
}

void WriteReleaseCandidateMatrixArtifact(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::string &artifact_json) {
  WriteText(BuildReleaseCandidateMatrixArtifactPath(out_dir, emit_prefix),
            artifact_json);
}
