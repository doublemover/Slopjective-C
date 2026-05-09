#include "driver/objc3_objc3_path.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "ast/objc3_ast.h"
#include "driver/objc3_driver_conformance_surface.h"
#include "driver/objc3_frontend_options.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_runtime_import_surface.h"

namespace fs = std::filesystem;

namespace {

bool TryDeriveConformancePublicationPath(const fs::path &report_path,
                                         fs::path &publication_path) {
  const std::string report_name = report_path.filename().string();
  const std::string suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  if (report_name.size() <= suffix.size() ||
      report_name.rfind(suffix) != report_name.size() - suffix.size()) {
    return false;
  }
  const std::string emit_prefix =
      report_name.substr(0u, report_name.size() - suffix.size());
  publication_path =
      BuildConformancePublicationArtifactPath(report_path.parent_path(),
                                             emit_prefix);
  return true;
}

bool TryDeriveConformanceEmitPrefix(const fs::path &report_path,
                                    std::string &emit_prefix) {
  const std::string report_name = report_path.filename().string();
  const std::string suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  if (report_name.size() <= suffix.size() ||
      report_name.rfind(suffix) != report_name.size() - suffix.size()) {
    emit_prefix.clear();
    return false;
  }
  emit_prefix = report_name.substr(0u, report_name.size() - suffix.size());
  return true;
}

}  // namespace

// error-model conformance gate anchor: lane-E freezes the current
// Part 6 slice by consuming the canonical integrated proof surface already
// published by the driver and artifact sidecars.
// async executable conformance gate anchor: lane-E must keep
// consuming this same emitted manifest/IR/object triplet for the runnable Part
// 7 slice instead of introducing an async-only publication channel.
// runnable async closeout matrix anchor: milestone closeout rows must
// keep consuming this same emitted manifest/IR/object triplet for the current
// Part 7 slice instead of a synthetic matrix-only publication path.
// task/executor conformance gate anchor: lane-E freezes the current
// runnable task/runtime slice by consuming the same published driver artifact
// surface while the broader front-door publication path remains fail-closed.
// runnable task/executor closeout matrix anchor: milestone closeout
// rows keep consuming this same driver artifact surface instead of inventing a
// matrix-only publication path for the current Part 7 task/runtime slice.
// strict concurrency conformance gate anchor: lane-E freezes the
// current runnable actor/isolation slice by consuming this same published
// driver artifact surface while the broader front-door actor publication path remains fail-closed.
// runnable actor/isolation closeout matrix anchor: milestone
// closeout rows keep consuming this same driver artifact surface instead of
// inventing a matrix-only publication path for the current Part 7 actor slice.
// strict system conformance gate anchor: lane-E freezes the current
// runnable Part 8 cleanup/resource/retainable slice by consuming this same
// published driver artifact surface while the broader front-door publication
// path remains fail-closed for deferred borrowed-lifetime runtime claims.
// runnable system-extension closeout matrix anchor: milestone
// closeout rows keep consuming this same driver artifact surface instead of
// inventing a matrix-only publication path for the current Part 8 slice.
// performance/dynamism conformance gate anchor: lane-E now freezes
// the current runnable Part 9 dispatch-control slice by consuming this same
// published driver artifact surface while the widened D002 runtime proof
// remains the canonical executable evidence boundary.
// runnable dispatch-control matrix closeout anchor: matrix rows keep
// consuming this same driver artifact surface while Part 9 closeout stays
// pinned to the existing D002 runtime proof instead of inventing a new lane-E
// publication channel.
// metaprogramming conformance gate anchor: lane-E freezes the
// currently supported Part 10 slice by consuming this same published driver
// artifact surface while the D002 live macro host-process/cache proof remains
// the canonical executable evidence boundary.
// runnable metaprogramming closeout matrix anchor: Part 10 closeout
// rows keep consuming this same driver artifact surface instead of inventing a
// parallel lane-E publication channel for derives/macros/property behaviors.

int RunObjc3ConformanceValidationPath(const Objc3CliOptions &cli_options) {
  std::string conformance_selection_error;
  if (!ValidateObjc3DriverConformanceSelection(cli_options,
                                               conformance_selection_error)) {
    std::cerr << conformance_selection_error << "\n";
    return 125;
  }

  if (!fs::exists(cli_options.validate_conformance_report_path)) {
    std::cerr << "conformance report not found: "
              << cli_options.validate_conformance_report_path.string() << "\n";
    return 2;
  }

  fs::path publication_path;
  std::string emit_prefix;
  if (!TryDeriveConformanceEmitPrefix(cli_options.validate_conformance_report_path,
                                      emit_prefix)) {
    std::cerr << "validated artifact must end with "
              << kObjc3VersionedConformanceReportLoweringArtifactSuffix << "\n";
    return 125;
  }
  if (!TryDeriveConformancePublicationPath(
          cli_options.validate_conformance_report_path, publication_path)) {
    std::cerr << "validated artifact must end with "
              << kObjc3VersionedConformanceReportLoweringArtifactSuffix << "\n";
    return 125;
  }
  if (!fs::exists(publication_path)) {
    std::cerr << "conformance publication artifact not found next to report: "
              << publication_path.string() << "\n";
    return 125;
  }
  std::string retired_claim_sidecar_error;
  if (!DiagnoseObjc3RetiredClaimSidecars(
          cli_options.validate_conformance_report_path.parent_path(), emit_prefix,
          retired_claim_sidecar_error)) {
    std::cerr << retired_claim_sidecar_error << "\n";
    return 125;
  }

  const std::string report_json =
      ReadText(cli_options.validate_conformance_report_path);
  const std::string publication_json = ReadText(publication_path);
  std::string validation_artifact_json;
  std::string validation_error;
  if (!TryBuildObjc3ConformanceClaimValidationArtifact(
          {.report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path =
               publication_path.filename().string()},
          report_json,
          publication_json,
          validation_artifact_json,
          validation_error)) {
    std::cerr << validation_error << "\n";
    return 125;
  }

  // versioning/conformance truth-gate anchor: this validation mode
  // is the integrated operator-side consumer of the D001 publication sidecar
  // and the C001/C002 lowered/runtime capability reports.
  WriteConformanceValidationArtifact(cli_options.out_dir,
                                     cli_options.emit_prefix,
                                     validation_artifact_json);

  std::string release_evidence_operation_json;
  std::string release_evidence_operation_error;
  if (!TryBuildObjc3ReleaseEvidenceOperationArtifact(
          {.report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path = publication_path.filename().string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                     cli_options.emit_prefix)
                   .filename()
                   .string(),
           .dashboard_artifact_path =
               BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                               cli_options.emit_prefix)
                   .filename()
                   .string()},
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
          {.report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path = publication_path.filename().string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                     cli_options.emit_prefix)
                   .filename()
                   .string(),
           .release_evidence_operation_artifact_path =
               BuildReleaseEvidenceOperationArtifactPath(
                   cli_options.out_dir, cli_options.emit_prefix)
                   .filename()
                   .string()},
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
           .report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path = publication_path.filename().string(),
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
           .report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path = publication_path.filename().string(),
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
