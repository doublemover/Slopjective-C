#include "driver/objc3_driver_conformance_publication.h"

#include <iostream>
#include <string>

#include "driver/objc3_driver_conformance_surface.h"
#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

int PublishObjc3DriverConformanceArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
          artifacts.versioned_conformance_report_lowering_summary)) {
    std::cerr << "versioned conformance-report lowering summary not ready\n";
    return 125;
  }
  if (artifacts.versioned_conformance_report_artifact_json.empty()) {
    std::cerr << "versioned conformance-report artifact payload missing\n";
    return 125;
  }
  WriteVersionedConformanceReportArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      artifacts.versioned_conformance_report_artifact_json);
  // versioning/conformance truth-gate anchor: lane-E freezes this emitted
  // lowered report plus the D001 publication sidecar and D002 validation mode
  // as one core/json-only fail-closed operator surface.
  // release/runtime-claim-matrix anchor: the published matrix must continue to
  // consume this exact native CLI report/publication/validation surface instead
  // of widening claims beyond the runnable core profile.
  if (cli_options.emit_objc3_conformance) {
    // D002 explicit operator parity: the native path already publishes the JSON
    // conformance sidecar by default, and this flag keeps that behavior
    // explicit/truthful for toolchain workflows that request the report
    // directly.
  }

  std::string conformance_publication_artifact_json;
  std::string conformance_publication_error;
  const Objc3DriverConformanceProfileSelection conformance_profiles =
      BuildObjc3DriverConformanceProfileSelection(cli_options);
  if (!TryBuildObjc3ConformanceReportPublicationArtifact(
          {.contract_id = "objc3c.driver.conformance.report.publication.v1",
           .schema_id = "objc3c-driver-conformance-publication-v1",
           .selected_profile = conformance_profiles.selected_profile,
           .selected_profile_supported =
               conformance_profiles.selected_profile_supported,
           .supported_profile_ids = conformance_profiles.supported_profile_ids,
           .rejected_profile_ids = conformance_profiles.rejected_profile_ids,
           .effective_language_profile = "canonical",
           .canonical_literal_rejection_diagnostics_enabled = false,
           .publication_model =
               "driver-publishes-lowered-conformance-sidecar-and-runtime-capability-sidecar-next-to-manifest",
           .publication_surface_kind = "native-cli",
           .fail_closed_diagnostic_model =
               "known-profiles-json-publication-remains-fail-closed-on-unsupported-formats-and-unknown-profiles",
           .lowered_report_contract_id =
               "objc3c.versioned.conformance.report.lowering.v1",
           .runtime_capability_contract_id =
               "objc3c.runtime.capability.reporting.v1",
           .public_conformance_schema_id = "objc3-conformance-report/v1",
           .advanced_feature_ops_contract_id =
               "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1",
           .advanced_feature_reporting_contract_id =
               "objc3c.tooling.feature.aware.conformance.report.emission.v1",
           .advanced_feature_release_evidence_contract_id =
               "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1",
           .ci_release_evidence_gate_script_path =
               "npm run objc3c -- check-release-evidence",
           .runbook_reference_path =
               "spec/conformance/release_evidence_gate_maintenance.md",
           .dashboard_schema_path =
               "schemas/objc3-conformance-dashboard-status-v1.schema.json",
           .advanced_feature_targeted_profile_ids =
               conformance_profiles.release_targeted_profile_ids,
           .report_artifact_relative_path =
               (cli_options.emit_prefix +
                kObjc3VersionedConformanceReportLoweringArtifactSuffix)},
          conformance_publication_artifact_json,
          conformance_publication_error)) {
    std::cerr << conformance_publication_error << "\n";
    return 125;
  }
  WriteConformancePublicationArtifact(cli_options.out_dir,
                                      cli_options.emit_prefix,
                                      conformance_publication_artifact_json);

  std::string advanced_feature_gate_artifact_json;
  std::string advanced_feature_gate_error;
  if (!TryBuildObjc3AdvancedFeatureGateArtifact(
          {.surface_kind = "native-cli",
           .report_artifact_path =
               (cli_options.emit_prefix +
                kObjc3VersionedConformanceReportLoweringArtifactSuffix),
           .publication_artifact_path =
               BuildConformancePublicationArtifactPath(cli_options.out_dir,
                                                       cli_options.emit_prefix)
                   .filename()
                   .string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                      cli_options.emit_prefix)
                   .filename()
                   .string(),
           .release_evidence_operation_artifact_path =
               BuildReleaseEvidenceOperationArtifactPath(
                   cli_options.out_dir, cli_options.emit_prefix)
                   .filename()
                   .string(),
           .dashboard_artifact_path =
               BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                                cli_options.emit_prefix)
                   .filename()
                   .string()},
          artifacts.versioned_conformance_report_artifact_json,
          conformance_publication_artifact_json,
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
          {.surface_kind = "native-cli",
           .report_artifact_path =
               (cli_options.emit_prefix +
                kObjc3VersionedConformanceReportLoweringArtifactSuffix),
           .publication_artifact_path =
               BuildConformancePublicationArtifactPath(cli_options.out_dir,
                                                       cli_options.emit_prefix)
                   .filename()
                   .string(),
           .advanced_feature_gate_artifact_path =
               BuildAdvancedFeatureGateArtifactPath(cli_options.out_dir,
                                                    cli_options.emit_prefix)
                   .filename()
                   .string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                      cli_options.emit_prefix)
                   .filename()
                   .string(),
           .release_evidence_operation_artifact_path =
               BuildReleaseEvidenceOperationArtifactPath(
                   cli_options.out_dir, cli_options.emit_prefix)
                   .filename()
                   .string(),
           .dashboard_artifact_path =
               BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                                cli_options.emit_prefix)
                   .filename()
                   .string()},
          artifacts.versioned_conformance_report_artifact_json,
          conformance_publication_artifact_json,
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
