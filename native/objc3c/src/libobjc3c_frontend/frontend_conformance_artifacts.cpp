#include "libobjc3c_frontend/frontend_conformance_artifacts.h"

#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "libobjc3c_frontend/frontend_conformance_publication_boundary.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_publication.h"

namespace objc3c::frontend {

namespace {

bool FrontendConformancePublicationIsActive(
    const objc3c_frontend_compile_result_t *result,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  return result != nullptr && result->status == OBJC3C_FRONTEND_STATUS_OK &&
         artifact_plan.has_out_dir;
}

}  // namespace

bool PublishFrontendConformanceReportArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  if (!FrontendConformancePublicationIsActive(result, artifact_plan)) {
    return true;
  }

  if (!IsFrontendConformanceReportLoweringSummaryReady(
          product.artifact_bundle
              .versioned_conformance_report_lowering_summary)) {
    SetFrontendPublicationError(
        context, result,
        "versioned conformance-report lowering summary not ready");
    return false;
  }
  if (product.artifact_bundle.versioned_conformance_report_artifact_json
          .empty()) {
    SetFrontendPublicationError(
        context, result,
        "versioned conformance-report artifact payload missing");
    return false;
  }

  const std::filesystem::path conformance_report_out =
      BuildVersionedConformanceReportArtifactPath(artifact_plan.out_dir,
                                                  artifact_plan.emit_prefix);
  const std::string conformance_report_artifact_filename =
      conformance_report_out.filename().string();
  if (!WriteFrontendTextArtifactOrError(
          context, result, conformance_report_out,
          product.artifact_bundle.versioned_conformance_report_artifact_json)) {
    return false;
  }

  std::string conformance_publication_artifact_json;
  std::string conformance_publication_error;
  if (!TryBuildObjc3ConformanceReportPublicationArtifact(
          {.contract_id = "objc3c.driver.conformance.report.publication.v1",
           .schema_id = "objc3c-driver-conformance-publication-v1",
           .selected_profile = "core",
           .selected_profile_supported = IsObjc3ClaimedConformanceProfile("core"),
           .supported_profile_ids = BuildObjc3ClaimedConformanceProfileIds(),
           .rejected_profile_ids = BuildObjc3RejectedConformanceProfileIds(),
           .effective_language_profile = "canonical",
           .canonical_literal_rejection_diagnostics_enabled = false,
           .publication_model =
               "driver-publishes-lowered-conformance-sidecar-and-runtime-capability-sidecar-next-to-manifest",
           .publication_surface_kind = "frontend-c-api",
           .fail_closed_diagnostic_model =
               "known-profiles-claimed-json-publication-remains-fail-closed-on-unsupported-formats-and-unknown-profiles",
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
               "scripts/check_release_evidence.py",
           .runbook_reference_path =
               "spec/conformance/release_evidence_gate_maintenance.md",
           .dashboard_schema_path =
               "schemas/objc3-conformance-dashboard-status-v1.schema.json",
           .advanced_feature_targeted_profile_ids =
               BuildObjc3ReleaseTargetedProfileIds(),
           .report_artifact_relative_path =
               conformance_report_artifact_filename},
          conformance_publication_artifact_json,
          conformance_publication_error)) {
    SetFrontendPublicationError(context, result,
                                conformance_publication_error);
    return false;
  }

  const std::filesystem::path conformance_publication_out =
      BuildConformancePublicationArtifactPath(artifact_plan.out_dir,
                                             artifact_plan.emit_prefix);
  if (!WriteFrontendTextArtifactOrError(context, result,
                                        conformance_publication_out,
                                        conformance_publication_artifact_json)) {
    return false;
  }

  std::string advanced_feature_gate_artifact_json;
  std::string advanced_feature_gate_error;
  if (!TryBuildObjc3AdvancedFeatureGateArtifact(
          {.surface_kind = "frontend-c-api",
           .report_artifact_path = conformance_report_artifact_filename,
           .publication_artifact_path =
               conformance_publication_out.filename().string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(artifact_plan.out_dir,
                                                      artifact_plan.emit_prefix)
                   .filename()
                   .string(),
           .release_evidence_operation_artifact_path =
               BuildReleaseEvidenceOperationArtifactPath(
                   artifact_plan.out_dir, artifact_plan.emit_prefix)
                   .filename()
                   .string(),
           .dashboard_artifact_path =
               BuildDashboardStatusArtifactPath(artifact_plan.out_dir,
                                                artifact_plan.emit_prefix)
                   .filename()
                   .string()},
          product.artifact_bundle.versioned_conformance_report_artifact_json,
          conformance_publication_artifact_json,
          advanced_feature_gate_artifact_json, advanced_feature_gate_error)) {
    SetFrontendPublicationError(context, result, advanced_feature_gate_error);
    return false;
  }

  const std::filesystem::path advanced_feature_gate_out =
      BuildAdvancedFeatureGateArtifactPath(artifact_plan.out_dir,
                                           artifact_plan.emit_prefix);
  if (!WriteFrontendTextArtifactOrError(context, result,
                                        advanced_feature_gate_out,
                                        advanced_feature_gate_artifact_json)) {
    return false;
  }

  std::string release_candidate_matrix_artifact_json;
  std::string release_candidate_matrix_error;
  if (!TryBuildObjc3ReleaseCandidateMatrixArtifact(
          {.surface_kind = "frontend-c-api",
           .report_artifact_path = conformance_report_artifact_filename,
           .publication_artifact_path =
               conformance_publication_out.filename().string(),
           .advanced_feature_gate_artifact_path =
               advanced_feature_gate_out.filename().string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(artifact_plan.out_dir,
                                                      artifact_plan.emit_prefix)
                   .filename()
                   .string(),
           .release_evidence_operation_artifact_path =
               BuildReleaseEvidenceOperationArtifactPath(
                   artifact_plan.out_dir, artifact_plan.emit_prefix)
                   .filename()
                   .string(),
           .dashboard_artifact_path =
               BuildDashboardStatusArtifactPath(artifact_plan.out_dir,
                                                artifact_plan.emit_prefix)
                   .filename()
                   .string()},
          product.artifact_bundle.versioned_conformance_report_artifact_json,
          conformance_publication_artifact_json,
          advanced_feature_gate_artifact_json,
          release_candidate_matrix_artifact_json,
          release_candidate_matrix_error)) {
    SetFrontendPublicationError(context, result, release_candidate_matrix_error);
    return false;
  }

  return WriteFrontendTextArtifactOrError(
      context, result,
      BuildReleaseCandidateMatrixArtifactPath(artifact_plan.out_dir,
                                              artifact_plan.emit_prefix),
      release_candidate_matrix_artifact_json);
}

}  // namespace objc3c::frontend
