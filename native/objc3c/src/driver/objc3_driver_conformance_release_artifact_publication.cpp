#include "driver/objc3_driver_conformance_release_artifact_publication.h"

#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process_conformance_contracts.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "lower/contracts/conformance_versioned_report_contracts.h"

namespace {

std::string Objc3DriverPublishedConformanceReportArtifactPath(
    const Objc3CliOptions &cli_options) {
  return cli_options.emit_prefix +
         kObjc3VersionedConformanceReportLoweringArtifactSuffix;
}

std::string Objc3DriverPublishedConformancePublicationArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildConformancePublicationArtifactPath(cli_options.out_dir,
                                                cli_options.emit_prefix)
      .filename()
      .string();
}

std::string Objc3DriverPublishedConformanceValidationArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                cli_options.emit_prefix)
      .filename()
      .string();
}

std::string Objc3DriverPublishedReleaseEvidenceOperationArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildReleaseEvidenceOperationArtifactPath(cli_options.out_dir,
                                                  cli_options.emit_prefix)
      .filename()
      .string();
}

std::string Objc3DriverPublishedDashboardStatusArtifactName(
    const Objc3CliOptions &cli_options) {
  return BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                          cli_options.emit_prefix)
      .filename()
      .string();
}

}  // namespace

int PublishObjc3DriverConformanceReleaseArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const std::string &conformance_publication_artifact_json) {
  std::string advanced_feature_gate_artifact_json;
  std::string advanced_feature_gate_error;
  if (!TryBuildObjc3AdvancedFeatureGateArtifact(
          {.surface_kind = "native-cli",
           .report_artifact_path =
               Objc3DriverPublishedConformanceReportArtifactPath(cli_options),
           .publication_artifact_path =
               Objc3DriverPublishedConformancePublicationArtifactName(cli_options),
           .validation_artifact_path =
               Objc3DriverPublishedConformanceValidationArtifactName(cli_options),
           .release_evidence_operation_artifact_path =
               Objc3DriverPublishedReleaseEvidenceOperationArtifactName(
                   cli_options),
           .dashboard_artifact_path =
               Objc3DriverPublishedDashboardStatusArtifactName(cli_options)},
          artifacts.versioned_conformance_report_artifact_json,
          conformance_publication_artifact_json,
          advanced_feature_gate_artifact_json,
          advanced_feature_gate_error)) {
    EmitObjc3DriverError(advanced_feature_gate_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  WriteAdvancedFeatureGateArtifact(cli_options.out_dir,
                                   cli_options.emit_prefix,
                                   advanced_feature_gate_artifact_json);

  std::string release_candidate_matrix_artifact_json;
  std::string release_candidate_matrix_error;
  if (!TryBuildObjc3ReleaseCandidateMatrixArtifact(
          {.surface_kind = "native-cli",
           .report_artifact_path =
               Objc3DriverPublishedConformanceReportArtifactPath(cli_options),
           .publication_artifact_path =
               Objc3DriverPublishedConformancePublicationArtifactName(cli_options),
           .advanced_feature_gate_artifact_path =
               BuildAdvancedFeatureGateArtifactPath(cli_options.out_dir,
                                                    cli_options.emit_prefix)
                   .filename()
                   .string(),
           .validation_artifact_path =
               Objc3DriverPublishedConformanceValidationArtifactName(cli_options),
           .release_evidence_operation_artifact_path =
               Objc3DriverPublishedReleaseEvidenceOperationArtifactName(
                   cli_options),
           .dashboard_artifact_path =
               Objc3DriverPublishedDashboardStatusArtifactName(cli_options)},
          artifacts.versioned_conformance_report_artifact_json,
          conformance_publication_artifact_json,
          advanced_feature_gate_artifact_json,
          release_candidate_matrix_artifact_json,
          release_candidate_matrix_error)) {
    EmitObjc3DriverError(release_candidate_matrix_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  WriteReleaseCandidateMatrixArtifact(cli_options.out_dir,
                                      cli_options.emit_prefix,
                                      release_candidate_matrix_artifact_json);
  return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
}
