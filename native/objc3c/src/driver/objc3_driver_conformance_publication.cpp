#include "driver/objc3_driver_conformance_publication.h"

#include <string>

#include "driver/objc3_driver_conformance_publication_sidecar.h"
#include "driver/objc3_driver_conformance_release_artifact_publication.h"
#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

int PublishObjc3DriverConformanceArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
          artifacts.versioned_conformance_report_lowering_summary)) {
    EmitObjc3DriverError("versioned conformance-report lowering summary not ready");
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }
  if (artifacts.versioned_conformance_report_artifact_json.empty()) {
    EmitObjc3DriverError("versioned conformance-report artifact payload missing");
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
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
  const int publication_status =
      PublishObjc3DriverConformancePublicationSidecar(
          cli_options, conformance_publication_artifact_json);
  if (publication_status != 0) {
    return publication_status;
  }
  return PublishObjc3DriverConformanceReleaseArtifacts(
      cli_options, artifacts, conformance_publication_artifact_json);
}
