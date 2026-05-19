#include "driver/objc3_driver_conformance_validation_artifact_names.h"

#include "io/objc3_artifact_paths.h"

Objc3DriverConformanceValidationArtifactNames
BuildObjc3DriverConformanceValidationArtifactNames(
    const Objc3CliOptions &cli_options,
    const std::filesystem::path &publication_path) {
  return {
      .report_artifact_path =
          cli_options.validate_conformance_report_path.filename().string(),
      .publication_artifact_path = publication_path.filename().string(),
      .validation_artifact_path =
          BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                 cli_options.emit_prefix)
              .filename()
              .string(),
      .release_evidence_operation_artifact_path =
          BuildReleaseEvidenceOperationArtifactPath(cli_options.out_dir,
                                                    cli_options.emit_prefix)
              .filename()
              .string(),
      .dashboard_artifact_path =
          BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                           cli_options.emit_prefix)
              .filename()
              .string(),
      .advanced_feature_gate_artifact_path =
          BuildAdvancedFeatureGateArtifactPath(cli_options.out_dir,
                                               cli_options.emit_prefix)
              .filename()
              .string(),
  };
}
