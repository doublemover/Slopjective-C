#include "driver/objc3_objc3_path.h"

#include <exception>
#include <filesystem>
#include <string>

#include "ast/objc3_ast.h"
#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_conformance_surface.h"
#include "driver/objc3_driver_conformance_validation_paths.h"
#include "driver/objc3_driver_conformance_validation_publication.h"
#include "driver/objc3_driver_status_codes.h"
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
    EmitObjc3DriverError(conformance_selection_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }

  if (!fs::exists(cli_options.validate_conformance_report_path)) {
    EmitObjc3DriverError(
        "conformance report not found: " +
        cli_options.validate_conformance_report_path.string());
    return Objc3DriverStatusValue(Objc3DriverStatusCode::kInputUnavailable);
  }

  fs::path publication_path;
  std::string emit_prefix;
  if (!TryDeriveObjc3DriverConformanceEmitPrefix(
          cli_options.validate_conformance_report_path, emit_prefix)) {
    EmitObjc3DriverError(
        std::string("validated artifact must end with ") +
        kObjc3VersionedConformanceReportLoweringArtifactSuffix);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  if (!TryDeriveObjc3DriverConformancePublicationPath(
          cli_options.validate_conformance_report_path, publication_path)) {
    EmitObjc3DriverError(
        std::string("validated artifact must end with ") +
        kObjc3VersionedConformanceReportLoweringArtifactSuffix);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  if (!fs::exists(publication_path)) {
    EmitObjc3DriverError(
        "conformance publication artifact not found next to report: " +
        publication_path.string());
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  std::string retired_claim_sidecar_error;
  if (!DiagnoseObjc3RetiredClaimSidecars(
          cli_options.validate_conformance_report_path.parent_path(), emit_prefix,
          retired_claim_sidecar_error)) {
    EmitObjc3DriverError(retired_claim_sidecar_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }

  const std::string report_json =
      ReadText(cli_options.validate_conformance_report_path);
  const std::string publication_json = ReadText(publication_path);

  // versioning/conformance truth-gate anchor: this validation mode
  // is the integrated operator-side consumer of the D001 publication sidecar
  // and the C001/C002 lowered/runtime capability reports.
  return PublishObjc3DriverConformanceValidationArtifacts(
      cli_options, publication_path, report_json, publication_json);
}
