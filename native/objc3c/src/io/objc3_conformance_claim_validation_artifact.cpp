#include "io/objc3_process_internal.h"
#include "io/objc3_conformance_release_artifact_document.h"

bool TryBuildObjc3ConformanceClaimValidationArtifact(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty()) {
    error = "conformance validation artifact inputs are incomplete";
    return false;
  }

  std::string report_schema_id;
  std::string report_contract_id;
  std::string effective_language_profile;
  std::string runtime_capability_contract_id;
  std::string public_conformance_schema_id;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::string publication_schema_id;
  std::string publication_contract_id;
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::string publication_surface_kind;
  std::string report_artifact_relative_path;
  std::string advanced_feature_ops_contract_id;
  std::string advanced_feature_reporting_contract_id;
  std::string advanced_feature_release_evidence_contract_id;
  std::vector<std::string> advanced_feature_targeted_profile_ids;
  std::string ci_release_evidence_gate_script_path;
  std::string runbook_reference_path;
  std::string dashboard_schema_path;
  const std::vector<std::string> expected_supported_profile_ids =
      BuildObjc3ClaimedConformanceProfileIds();
  const std::vector<std::string> expected_rejected_profile_ids =
      BuildObjc3RejectedConformanceProfileIds();
  const std::vector<std::string> expected_targeted_profile_ids =
      BuildObjc3ReleaseTargetedProfileIds();
  JsonValue report_document;
  JsonValue publication_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error)) {
    return false;
  }

  if (!TryGetJsonStringField(report_document, "schema_id", report_schema_id) ||
      report_schema_id != "objc3c-versioned-conformance-report-v1") {
    error = "invalid conformance report schema_id";
    return false;
  }
  if (!TryGetJsonStringField(report_document, "contract_id", report_contract_id) ||
      report_contract_id != "objc3c.versioned.conformance.report.lowering.v1") {
    error = "invalid conformance report contract_id";
    return false;
  }
  if (!TryGetJsonStringField(report_document, "effective_language_profile",
                             effective_language_profile)) {
    error = "missing effective_language_profile in conformance report";
    return false;
  }
  if (!TryGetJsonBoolField(
          report_document, "canonical_literal_rejection_diagnostics_enabled",
          canonical_literal_rejection_diagnostics_enabled)) {
    error = "missing canonical_literal_rejection_diagnostics_enabled in conformance report";
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document, {"runtime_capability_report", "contract_id"},
          "objc3c.runtime.capability.reporting.v1")) {
    error = "runtime_capability_report payload is missing or drifted";
    return false;
  }
  runtime_capability_contract_id =
      "objc3c.runtime.capability.reporting.v1";
  if (!JsonStringFieldEquals(
          report_document, {"public_conformance_report", "schema_id"},
          "objc3-conformance-report/v1")) {
    error = "public_conformance_report payload is missing or drifted";
    return false;
  }
  public_conformance_schema_id = "objc3-conformance-report/v1";
  if (!JsonStringFieldEquals(
          report_document, {"advanced_feature_reporting", "contract_id"},
          kObjc3AdvancedFeatureReportingContractId)) {
    error = "advanced_feature_reporting payload is missing or drifted";
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "contract_id"},
          kObjc3AdvancedFeatureReleaseEvidenceContractId)) {
    error = "advanced_feature_release_evidence payload is missing or drifted";
    return false;
  }

  if (!TryGetJsonStringField(publication_document, "schema_id",
                             publication_schema_id) ||
      publication_schema_id != "objc3c-driver-conformance-publication-v1") {
    error = "invalid conformance publication schema_id";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "contract_id",
                             publication_contract_id) ||
      publication_contract_id !=
          "objc3c.driver.conformance.report.publication.v1") {
    error = "invalid conformance publication contract_id";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "selected_profile",
                             selected_profile)) {
    error = "selected_profile is missing from conformance publication";
    return false;
  }
  if (!TryGetJsonBoolField(publication_document, "selected_profile_supported",
                           selected_profile_supported) ||
      selected_profile_supported !=
          IsObjc3ClaimedConformanceProfile(selected_profile)) {
    error = "selected_profile_supported drifted from the live claim policy";
    return false;
  }
  if (!TryGetJsonStringArrayField(publication_document, "supported_profile_ids",
                                  supported_profile_ids) ||
      supported_profile_ids != expected_supported_profile_ids) {
    error = "supported_profile_ids drifted from the live claim policy";
    return false;
  }
  if (!TryGetJsonStringArrayField(publication_document, "rejected_profile_ids",
                                  rejected_profile_ids) ||
      rejected_profile_ids != expected_rejected_profile_ids) {
    error = "rejected_profile_ids drifted from the live claim policy";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "publication_surface_kind",
                             publication_surface_kind)) {
    error = "missing publication_surface_kind in conformance publication";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "report_artifact",
                             report_artifact_relative_path) ||
      report_artifact_relative_path !=
          std::filesystem::path(inputs.report_artifact_path).filename().string()) {
    error = "publication report_artifact does not match the validated report path";
    return false;
  }
  if (!TryGetJsonStringField(publication_document,
                             "advanced_feature_ops_contract_id",
                             advanced_feature_ops_contract_id) ||
      advanced_feature_ops_contract_id != kObjc3AdvancedFeatureOpsContractId) {
    error = "advanced_feature_ops_contract_id drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document,
                             "advanced_feature_reporting_contract_id",
                             advanced_feature_reporting_contract_id) ||
      advanced_feature_reporting_contract_id !=
          kObjc3AdvancedFeatureReportingContractId) {
    error = "advanced_feature_reporting_contract_id drifted";
    return false;
  }
  if (!TryGetJsonStringField(
          publication_document, "advanced_feature_release_evidence_contract_id",
          advanced_feature_release_evidence_contract_id) ||
      advanced_feature_release_evidence_contract_id !=
          kObjc3AdvancedFeatureReleaseEvidenceContractId) {
    error = "advanced_feature_release_evidence_contract_id drifted";
    return false;
  }
  if (!TryGetJsonStringArrayField(publication_document,
                                  "advanced_feature_targeted_profile_ids",
                                  advanced_feature_targeted_profile_ids) ||
      advanced_feature_targeted_profile_ids != expected_targeted_profile_ids) {
    error = "advanced_feature_targeted_profile_ids drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document,
                             "ci_release_evidence_gate_script_path",
                             ci_release_evidence_gate_script_path) ||
      ci_release_evidence_gate_script_path !=
          kObjc3AdvancedFeatureEvidenceGateScriptPath) {
    error = "ci_release_evidence_gate_script_path drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "runbook_reference_path",
                             runbook_reference_path) ||
      runbook_reference_path !=
          kObjc3AdvancedFeatureEvidenceRunbookPath) {
    error = "runbook_reference_path drifted";
    return false;
  }
  if (!TryGetJsonStringField(publication_document, "dashboard_schema_path",
                             dashboard_schema_path) ||
      dashboard_schema_path !=
          kObjc3AdvancedFeatureDashboardSchemaPath) {
    error = "dashboard_schema_path drifted";
    return false;
  }
  if (!JsonStringFieldEquals(publication_document,
                             {"fail_closed_diagnostic_model"},
                             kObjc3ConformancePublicationFailClosedDiagnosticModel)) {
    error = "publication fail_closed_diagnostic_model drifted";
    return false;
  }

  artifact_json = BuildObjc3ConformanceClaimValidationArtifactDocumentJson(
      inputs, report_schema_id, report_contract_id,
      runtime_capability_contract_id, public_conformance_schema_id,
      advanced_feature_targeted_profile_ids, ci_release_evidence_gate_script_path,
      runbook_reference_path, dashboard_schema_path, selected_profile,
      selected_profile_supported, supported_profile_ids, rejected_profile_ids,
      effective_language_profile, canonical_literal_rejection_diagnostics_enabled,
      publication_surface_kind);
  return true;
}

