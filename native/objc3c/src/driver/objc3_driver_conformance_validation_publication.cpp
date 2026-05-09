#include "driver/objc3_driver_conformance_validation_publication.h"

#include <iostream>

#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"

namespace {

std::string Objc3DriverValidationReportArtifactName(
    const Objc3CliOptions &cli_options) {
  return cli_options.validate_conformance_report_path.filename().string();
}

std::string Objc3DriverValidationArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                cli_options.emit_prefix)
      .filename()
      .string();
}

std::string Objc3DriverReleaseEvidenceOperationArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildReleaseEvidenceOperationArtifactPath(cli_options.out_dir,
                                                  cli_options.emit_prefix)
      .filename()
      .string();
}

std::string Objc3DriverDashboardStatusArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                          cli_options.emit_prefix)
      .filename()
      .string();
}

std::string Objc3DriverAdvancedFeatureGateArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildAdvancedFeatureGateArtifactPath(cli_options.out_dir,
                                             cli_options.emit_prefix)
      .filename()
      .string();
}

}  // namespace

int PublishObjc3DriverConformanceValidationArtifacts(
    const Objc3CliOptions &cli_options,
    const std::filesystem::path &publication_path,
    const std::string &report_json,
    const std::string &publication_json) {
  const std::string report_artifact_path =
      Objc3DriverValidationReportArtifactName(cli_options);
  const std::string publication_artifact_path =
      publication_path.filename().string();
  const std::string validation_artifact_path =
      Objc3DriverValidationArtifactName(cli_options);
  const std::string release_evidence_operation_artifact_path =
      Objc3DriverReleaseEvidenceOperationArtifactName(cli_options);
  const std::string dashboard_artifact_path =
      Objc3DriverDashboardStatusArtifactName(cli_options);

  std::string validation_artifact_json;
  std::string validation_error;
  if (!TryBuildObjc3ConformanceClaimValidationArtifact(
          {.report_artifact_path = report_artifact_path,
           .publication_artifact_path = publication_artifact_path},
          report_json,
          publication_json,
          validation_artifact_json,
          validation_error)) {
    std::cerr << validation_error << "\n";
    return 125;
  }
  WriteConformanceValidationArtifact(cli_options.out_dir,
                                     cli_options.emit_prefix,
                                     validation_artifact_json);

  std::string release_evidence_operation_json;
  std::string release_evidence_operation_error;
  if (!TryBuildObjc3ReleaseEvidenceOperationArtifact(
          {.report_artifact_path = report_artifact_path,
           .publication_artifact_path = publication_artifact_path,
           .validation_artifact_path = validation_artifact_path,
           .dashboard_artifact_path = dashboard_artifact_path},
          report_json,
          publication_json,
          validation_artifact_json,
          release_evidence_operation_json,
          release_evidence_operation_error)) {
    std::cerr << release_evidence_operation_error << "\n";
    return 125;
  }
  WriteReleaseEvidenceOperationArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        release_evidence_operation_json);

  std::string dashboard_status_json;
  std::string dashboard_status_error;
  if (!TryBuildObjc3DashboardStatusArtifact(
          {.report_artifact_path = report_artifact_path,
           .publication_artifact_path = publication_artifact_path,
           .validation_artifact_path = validation_artifact_path,
           .release_evidence_operation_artifact_path =
               release_evidence_operation_artifact_path},
          report_json,
          publication_json,
          validation_artifact_json,
          release_evidence_operation_json,
          dashboard_status_json,
          dashboard_status_error)) {
    std::cerr << dashboard_status_error << "\n";
    return 125;
  }
  WriteDashboardStatusArtifact(cli_options.out_dir,
                               cli_options.emit_prefix,
                               dashboard_status_json);

  std::string advanced_feature_gate_artifact_json;
  std::string advanced_feature_gate_error;
  if (!TryBuildObjc3AdvancedFeatureGateArtifact(
          {.surface_kind = "native-cli-validation",
           .report_artifact_path = report_artifact_path,
           .publication_artifact_path = publication_artifact_path,
           .validation_artifact_path = validation_artifact_path,
           .release_evidence_operation_artifact_path =
               release_evidence_operation_artifact_path,
           .dashboard_artifact_path = dashboard_artifact_path},
          report_json,
          publication_json,
          advanced_feature_gate_artifact_json,
          advanced_feature_gate_error)) {
    std::cerr << advanced_feature_gate_error << "\n";
    return 125;
  }
  WriteAdvancedFeatureGateArtifact(cli_options.out_dir,
                                   cli_options.emit_prefix,
                                   advanced_feature_gate_artifact_json);

  std::string release_candidate_matrix_artifact_json;
  std::string release_candidate_matrix_error;
  if (!TryBuildObjc3ReleaseCandidateMatrixArtifact(
          {.surface_kind = "native-cli-validation",
           .report_artifact_path = report_artifact_path,
           .publication_artifact_path = publication_artifact_path,
           .advanced_feature_gate_artifact_path =
               Objc3DriverAdvancedFeatureGateArtifactName(cli_options),
           .validation_artifact_path = validation_artifact_path,
           .release_evidence_operation_artifact_path =
               release_evidence_operation_artifact_path,
           .dashboard_artifact_path = dashboard_artifact_path},
          report_json,
          publication_json,
          advanced_feature_gate_artifact_json,
          release_candidate_matrix_artifact_json,
          release_candidate_matrix_error)) {
    std::cerr << release_candidate_matrix_error << "\n";
    return 125;
  }
  WriteReleaseCandidateMatrixArtifact(cli_options.out_dir,
                                      cli_options.emit_prefix,
                                      release_candidate_matrix_artifact_json);
  return 0;
}
