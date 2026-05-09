#include "driver/objc3_driver_conformance_publication_sidecar.h"

#include <iostream>

#include "driver/objc3_driver_conformance_surface.h"
#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

int PublishObjc3DriverConformancePublicationSidecar(
    const Objc3CliOptions &cli_options,
    std::string &conformance_publication_artifact_json) {
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
  return 0;
}
