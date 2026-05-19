#include "driver/objc3_driver_conformance_validation_publication.h"

#include "driver/objc3_driver_conformance_validation_artifact_names.h"
#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process_conformance_contracts.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"

int PublishObjc3DriverConformanceValidationArtifacts(
    const Objc3CliOptions &cli_options,
    const std::filesystem::path &publication_path,
    const std::string &report_json,
    const std::string &publication_json) {
  const Objc3DriverConformanceValidationArtifactNames artifact_names =
      BuildObjc3DriverConformanceValidationArtifactNames(cli_options,
                                                         publication_path);

  std::string validation_artifact_json;
  std::string validation_error;
  if (!TryBuildObjc3ConformanceClaimValidationArtifact(
          {.report_artifact_path = artifact_names.report_artifact_path,
           .publication_artifact_path =
               artifact_names.publication_artifact_path},
          report_json,
          publication_json,
          validation_artifact_json,
          validation_error)) {
    EmitObjc3DriverError(validation_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  WriteConformanceValidationArtifact(cli_options.out_dir,
                                     cli_options.emit_prefix,
                                     validation_artifact_json);

  std::string release_evidence_operation_json;
  std::string release_evidence_operation_error;
  if (!TryBuildObjc3ReleaseEvidenceOperationArtifact(
          {.report_artifact_path = artifact_names.report_artifact_path,
           .publication_artifact_path =
               artifact_names.publication_artifact_path,
           .validation_artifact_path =
               artifact_names.validation_artifact_path,
           .dashboard_artifact_path = artifact_names.dashboard_artifact_path},
          report_json,
          publication_json,
          validation_artifact_json,
          release_evidence_operation_json,
          release_evidence_operation_error)) {
    EmitObjc3DriverError(release_evidence_operation_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  WriteReleaseEvidenceOperationArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        release_evidence_operation_json);

  std::string dashboard_status_json;
  std::string dashboard_status_error;
  if (!TryBuildObjc3DashboardStatusArtifact(
          {.report_artifact_path = artifact_names.report_artifact_path,
           .publication_artifact_path =
               artifact_names.publication_artifact_path,
           .validation_artifact_path =
               artifact_names.validation_artifact_path,
           .release_evidence_operation_artifact_path =
               artifact_names.release_evidence_operation_artifact_path},
          report_json,
          publication_json,
          validation_artifact_json,
          release_evidence_operation_json,
          dashboard_status_json,
          dashboard_status_error)) {
    EmitObjc3DriverError(dashboard_status_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  WriteDashboardStatusArtifact(cli_options.out_dir,
                               cli_options.emit_prefix,
                               dashboard_status_json);

  std::string advanced_feature_gate_artifact_json;
  std::string advanced_feature_gate_error;
  if (!TryBuildObjc3AdvancedFeatureGateArtifact(
          {.surface_kind = "native-cli-validation",
           .report_artifact_path = artifact_names.report_artifact_path,
           .publication_artifact_path =
               artifact_names.publication_artifact_path,
           .validation_artifact_path =
               artifact_names.validation_artifact_path,
           .release_evidence_operation_artifact_path =
               artifact_names.release_evidence_operation_artifact_path,
           .dashboard_artifact_path = artifact_names.dashboard_artifact_path},
          report_json,
          publication_json,
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
          {.surface_kind = "native-cli-validation",
           .report_artifact_path = artifact_names.report_artifact_path,
           .publication_artifact_path =
               artifact_names.publication_artifact_path,
           .advanced_feature_gate_artifact_path =
               artifact_names.advanced_feature_gate_artifact_path,
           .validation_artifact_path =
               artifact_names.validation_artifact_path,
           .release_evidence_operation_artifact_path =
               artifact_names.release_evidence_operation_artifact_path,
           .dashboard_artifact_path = artifact_names.dashboard_artifact_path},
          report_json,
          publication_json,
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
