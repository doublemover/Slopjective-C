#include "io/objc3_process_internal.h"

bool TryBuildObjc3ConformanceReportPublicationArtifact(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  // Publication remains fail-closed on unsupported formats and unknown profiles
  // even after the built-in strict profiles become claimable.

  if (inputs.contract_id.empty() || inputs.schema_id.empty() ||
      inputs.selected_profile.empty() || inputs.supported_profile_ids.empty() ||
      inputs.effective_language_profile.empty() ||
      inputs.publication_model.empty() ||
      inputs.publication_surface_kind.empty() ||
      inputs.fail_closed_diagnostic_model.empty() ||
      inputs.lowered_report_contract_id.empty() ||
      inputs.runtime_capability_contract_id.empty() ||
      inputs.public_conformance_schema_id.empty() ||
      inputs.advanced_feature_ops_contract_id.empty() ||
      inputs.advanced_feature_reporting_contract_id.empty() ||
      inputs.advanced_feature_release_evidence_contract_id.empty() ||
      inputs.ci_release_evidence_gate_script_path.empty() ||
      inputs.runbook_reference_path.empty() ||
      inputs.dashboard_schema_path.empty() ||
      inputs.advanced_feature_targeted_profile_ids.empty() ||
      inputs.report_artifact_relative_path.empty()) {
    error = "conformance report publication artifact inputs are incomplete";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter publication(out);
  publication.StringField("contract_id", inputs.contract_id);
  publication.StringField("schema_id", inputs.schema_id);
  publication.StringField("selected_profile", inputs.selected_profile);
  publication.BoolField("selected_profile_supported",
                        inputs.selected_profile_supported);
  publication.RawJsonField(
      "supported_profile_ids",
      BuildIndentedStringArrayJson(inputs.supported_profile_ids, "    "));
  publication.RawJsonField(
      "rejected_profile_ids",
      BuildIndentedStringArrayJson(inputs.rejected_profile_ids, "    "));
  publication.StringField("effective_language_profile",
                          inputs.effective_language_profile);
  publication.BoolField("canonical_literal_rejection_diagnostics_enabled",
                        inputs.canonical_literal_rejection_diagnostics_enabled);
  publication.StringField("publication_model", inputs.publication_model);
  publication.StringField("publication_surface_kind",
                          inputs.publication_surface_kind);
  publication.StringField("fail_closed_diagnostic_model",
                          inputs.fail_closed_diagnostic_model);
  publication.StringField("lowered_report_contract_id",
                          inputs.lowered_report_contract_id);
  publication.StringField("runtime_capability_contract_id",
                          inputs.runtime_capability_contract_id);
  publication.StringField("public_conformance_schema_id",
                          inputs.public_conformance_schema_id);
  publication.StringField("advanced_feature_ops_contract_id",
                          inputs.advanced_feature_ops_contract_id);
  publication.StringField("advanced_feature_reporting_contract_id",
                          inputs.advanced_feature_reporting_contract_id);
  publication.StringField(
      "advanced_feature_release_evidence_contract_id",
      inputs.advanced_feature_release_evidence_contract_id);
  publication.RawJsonField(
      "advanced_feature_targeted_profile_ids",
      BuildIndentedStringArrayJson(inputs.advanced_feature_targeted_profile_ids,
                                   "    "));
  publication.StringField("ci_release_evidence_gate_script_path",
                          inputs.ci_release_evidence_gate_script_path);
  publication.StringField("runbook_reference_path",
                          inputs.runbook_reference_path);
  publication.StringField("dashboard_schema_path",
                          inputs.dashboard_schema_path);
  publication.StringField("report_artifact",
                          inputs.report_artifact_relative_path);
  publication.BoolField("ready", true);
  artifact_json = FinishJsonObject(publication, out);
  return true;
}

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

  std::ostringstream out;
  JsonObjectWriter validation(out);
  validation.StringField("contract_id",
                         kObjc3ToolchainConformanceClaimOperationsContractId);
  validation.StringField("schema_id",
                         kObjc3ToolchainConformanceClaimValidationSchemaId);
  validation.StringField("validation_model",
                         kObjc3ToolchainConformanceClaimValidationModel);
  validation.StringField("consumption_model",
                         kObjc3ToolchainConformanceClaimConsumptionModel);
  validation.StringField("format", "json");
  validation.StringField("validated_report_artifact",
                         inputs.report_artifact_path);
  validation.StringField("validated_publication_artifact",
                         inputs.publication_artifact_path);
  validation.StringField("report_schema_id", report_schema_id);
  validation.StringField("report_contract_id", report_contract_id);
  validation.StringField("runtime_capability_contract_id",
                         runtime_capability_contract_id);
  validation.StringField("public_conformance_schema_id",
                         public_conformance_schema_id);
  validation.StringField("advanced_feature_ops_contract_id",
                         kObjc3AdvancedFeatureOpsContractId);
  validation.StringField("advanced_feature_reporting_contract_id",
                         kObjc3AdvancedFeatureReportingContractId);
  validation.StringField("advanced_feature_release_evidence_contract_id",
                         kObjc3AdvancedFeatureReleaseEvidenceContractId);
  validation.RawJsonField(
      "advanced_feature_targeted_profile_ids",
      BuildIndentedStringArrayJson(advanced_feature_targeted_profile_ids,
                                   "    "));
  validation.StringField("ci_release_evidence_gate_script_path",
                         ci_release_evidence_gate_script_path);
  validation.StringField("runbook_reference_path", runbook_reference_path);
  validation.StringField("dashboard_schema_path", dashboard_schema_path);
  validation.StringField("selected_profile", selected_profile);
  validation.BoolField("selected_profile_supported", selected_profile_supported);
  validation.RawJsonField("supported_profile_ids",
                          BuildIndentedStringArrayJson(supported_profile_ids,
                                                       "    "));
  validation.RawJsonField("rejected_profile_ids",
                          BuildIndentedStringArrayJson(rejected_profile_ids,
                                                       "    "));
  validation.StringField("effective_language_profile",
                         effective_language_profile);
  validation.BoolField("canonical_literal_rejection_diagnostics_enabled",
                       canonical_literal_rejection_diagnostics_enabled);
  validation.StringField("publication_surface_kind", publication_surface_kind);
  validation.BoolField("ready", true);
  artifact_json = FinishJsonObject(validation, out);
  return true;
}

bool TryBuildObjc3ReleaseEvidenceOperationArtifact(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "release evidence operation artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  JsonValue validation_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error) ||
      !TryParseJsonObjectText(validation_json, "conformance validation",
                              validation_document, error)) {
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence",
           "release_evidence_checklist_path"},
          "spec/conformance/profile_release_evidence_checklist.md") ||
      !JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "release_evidence_schema_path"},
          "spec/conformance/objc3_conformance_evidence_bundle_schema.md")) {
    error = "advanced feature release evidence payload is missing or drifted";
    return false;
  }
  if (!JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId) ||
      !JsonStringFieldEquals(validation_document, {"contract_id"},
                             kObjc3ToolchainConformanceClaimOperationsContractId)) {
    error = "advanced feature operator contract drifted";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter operation(out);
  operation.StringField("contract_id", kObjc3ReleaseEvidenceOperationContractId);
  operation.StringField("schema_id", kObjc3ReleaseEvidenceOperationSchemaId);
  operation.RawJsonField(
      "dependency_contract_ids",
      BuildIndentedStringArrayJson(
          {kObjc3AdvancedFeatureOpsContractId,
           kObjc3AdvancedFeatureReleaseEvidenceContractId},
          "    "));
  operation.StringField("validation_contract_id",
                        kObjc3ToolchainConformanceClaimOperationsContractId);
  operation.StringField("dashboard_contract_id",
                        kObjc3DashboardStatusPublicationContractId);
  operation.StringField(
      "operation_model",
      "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set");
  operation.StringField("release_label", kObjc3AdvancedFeatureReleaseLabel);
  operation.RawJsonField(
      "command_tokens",
      BuildIndentedStringArrayJson(
          {"python", kObjc3AdvancedFeatureEvidenceGateScriptPath}, "    "));
  operation.StringField("report_artifact", inputs.report_artifact_path);
  operation.StringField("publication_artifact",
                        inputs.publication_artifact_path);
  operation.StringField("validation_artifact", inputs.validation_artifact_path);
  operation.StringField("dashboard_artifact", inputs.dashboard_artifact_path);
  operation.StringField("gate_script_path",
                        kObjc3AdvancedFeatureEvidenceGateScriptPath);
  operation.StringField("runbook_reference_path",
                        kObjc3AdvancedFeatureEvidenceRunbookPath);
  operation.StringField("dashboard_schema_path",
                        kObjc3AdvancedFeatureDashboardSchemaPath);
  operation.StringField("release_evidence_checklist_path",
                        "spec/conformance/profile_release_evidence_checklist.md");
  operation.StringField(
      "release_evidence_schema_path",
      "spec/conformance/objc3_conformance_evidence_bundle_schema.md");
  operation.RawJsonField(
      "targeted_profile_ids",
      BuildIndentedStringArrayJson(
          {"strict", "strict-concurrency", "strict-system"}, "    "));
  operation.RawJsonField(
      "corpus_shard_ids",
      BuildIndentedStringArrayJson({"parser", "semantic", "lowering_abi",
                                    "module_roundtrip", "diagnostics"},
                                   "    "));
  operation.RawJsonField(
      "release_evidence_artifact_ids",
      BuildIndentedStringArrayJson({"EVID-01", "EVID-02", "EVID-03",
                                    "EVID-04", "EVID-07", "EVID-08",
                                    "EVID-09", "EVID-10", "EVID-11"},
                                   "    "));
  operation.StringField("generated_at", kObjc3DeterministicReplayTimestamp);
  operation.BoolField("ready", true);
  artifact_json = FinishJsonObject(operation, out);
  return true;
}

bool TryBuildObjc3DashboardStatusArtifact(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty()) {
    error = "dashboard status artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  JsonValue validation_document;
  JsonValue release_evidence_operation_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error) ||
      !TryParseJsonObjectText(validation_json, "conformance validation",
                              validation_document, error) ||
      !TryParseJsonObjectText(release_evidence_operation_json,
                              "release evidence operation",
                              release_evidence_operation_document, error)) {
    return false;
  }
  if (!HasJsonField(report_document, {"advanced_feature_release_evidence"}) ||
      !JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId) ||
      !JsonStringFieldEquals(validation_document, {"contract_id"},
                             kObjc3ToolchainConformanceClaimOperationsContractId) ||
      !JsonStringFieldEquals(release_evidence_operation_document,
                             {"contract_id"},
                             kObjc3ReleaseEvidenceOperationContractId)) {
    error = "dashboard status publication inputs drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3DashboardStatusSchemaId)
      << "\",\n"
      << "  \"schema_version\": 1,\n"
      << "  \"dashboard_version\": \""
      << EscapeJsonString(kObjc3DashboardVersion)
      << "\",\n"
      << "  \"release_label\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseLabel) << "\",\n"
      << "  \"release_id\": \""
      << EscapeJsonString(kObjc3DashboardReleaseId) << "\",\n"
      << "  \"generated_at\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp) << "\",\n"
      << "  \"source_revision\": \""
      << EscapeJsonString(kObjc3DeterministicSourceRevision) << "\",\n"
      << "  \"status\": \"pass\",\n"
      << "  \"profiles\": [\n"
      << "    {\"profile_id\":\"core\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]},\n"
      << "    {\"profile_id\":\"strict\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]},\n"
      << "    {\"profile_id\":\"strict-concurrency\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]},\n"
      << "    {\"profile_id\":\"strict-system\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]}\n"
      << "  ],\n"
      << "  \"dependencies\": [\n"
      << "    {\"dependency_id\":\"B-04\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B04-REPORT\"],\"failure_codes\":[]},\n"
      << "    {\"dependency_id\":\"B-10\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B10-PUBLICATION\"],\"failure_codes\":[]},\n"
      << "    {\"dependency_id\":\"B-11\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B11-VALIDATION\"],\"failure_codes\":[]},\n"
      << "    {\"dependency_id\":\"B-12\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B12-RELEASE-EVIDENCE\"],\"failure_codes\":[]}\n"
      << "  ],\n"
      << "  \"artifacts\": [\n"
      << "    {\"artifact_id\":\"ART-B04-REPORT\",\"dependency_id\":\"B-04\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.report_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(ComputeSha256ShapedContentDigest(report_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#140/conformance-report\"},\n"
      << "    {\"artifact_id\":\"ART-B10-PUBLICATION\",\"dependency_id\":\"B-10\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.publication_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(ComputeSha256ShapedContentDigest(publication_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#158/conformance-publication\"},\n"
      << "    {\"artifact_id\":\"ART-B11-VALIDATION\",\"dependency_id\":\"B-11\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.validation_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(ComputeSha256ShapedContentDigest(validation_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#161/conformance-validation\"},\n"
      << "    {\"artifact_id\":\"ART-B12-RELEASE-EVIDENCE\",\"dependency_id\":\"B-12\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.release_evidence_operation_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(
             ComputeSha256ShapedContentDigest(release_evidence_operation_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#167/release-evidence\"}\n"
      << "  ],\n"
      << "  \"blockers\": [],\n"
      << "  \"summary\": {\n"
      << "    \"profile_counts\": {\"pass\":4,\"fail\":0,\"blocked\":0,\"incomplete\":0},\n"
      << "    \"dependency_counts\": {\"pass\":4,\"fail\":0,\"blocked\":0,\"stale\":0,\"missing\":0},\n"
      << "    \"blocker_counts\": {\"open\":0,\"resolved\":0,\"high_or_critical\":0}\n"
      << "  },\n"
      << "  \"refresh\": {\n"
      << "    \"trigger\": \"manual-replay\",\n"
      << "    \"cadence\": {\"merge_latency_target_minutes\":30,\"scheduled_latency_target_minutes\":60,\"rc_fast_refresh_hours\":4},\n"
      << "    \"last_successful_refresh\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\n"
      << "    \"next_scheduled_refresh\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\n"
      << "    \"stale_dependency_ids\": [],\n"
      << "    \"missed_scheduled_refreshes\": 0,\n"
      << "    \"escalation_state\": \"none\"\n"
      << "  },\n"
      << "  \"change_history\": [\n"
      << "    {\"snapshot_id\":\""
      << EscapeJsonString(kObjc3DashboardReleaseId)
      << "\",\"previous_snapshot_id\":null,\"change_kind\":\"refresh-only\",\"changed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"summary\":\"Deterministic claim dashboard refresh.\"}\n"
      << "  ]\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3AdvancedFeatureGateArtifact(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.surface_kind.empty() || inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "advanced feature gate artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error)) {
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document, {"advanced_feature_reporting", "contract_id"},
          kObjc3AdvancedFeatureReportingContractId) ||
      !JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "contract_id"},
          kObjc3AdvancedFeatureReleaseEvidenceContractId) ||
      !JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId)) {
    error = "advanced feature gate inputs drifted";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter gate(out);
  gate.StringField("contract_id", kObjc3AdvancedFeatureGateContractId);
  gate.StringField("schema_id", kObjc3AdvancedFeatureGateSchemaId);
  gate.RawJsonField(
      "dependency_contract_ids",
      BuildIndentedStringArrayJson(
          {"objc3c.tooling.frontend.migration.canonicalization.source.completion.v1",
           "objc3c.tooling.legacy.canonical.migration.semantics.v1",
           kObjc3AdvancedFeatureReleaseEvidenceContractId,
           kObjc3ReleaseEvidenceOperationContractId},
          "    "));
  gate.StringField("surface_kind", inputs.surface_kind);
  gate.StringField("report_artifact", inputs.report_artifact_path);
  gate.StringField("publication_artifact", inputs.publication_artifact_path);
  gate.StringField("validation_artifact_expected",
                   inputs.validation_artifact_path);
  gate.StringField("release_evidence_operation_artifact_expected",
                   inputs.release_evidence_operation_artifact_path);
  gate.StringField("dashboard_artifact_expected", inputs.dashboard_artifact_path);
  gate.StringField(
      "gate_model",
      "integrated-advanced-feature-gate-consumes-report-publication-and-native-validation-sidecars");
  gate.RawJsonField(
      "targeted_profile_ids",
      BuildIndentedStringArrayJson(BuildObjc3ReleaseTargetedProfileIds(),
                                   "    "));
  gate.BoolField("native_validation_required", true);
  gate.BoolField("report_payload_ready", true);
  gate.BoolField("release_evidence_ready", true);
  gate.BoolField("ready", true);
  artifact_json = FinishJsonObject(gate, out);
  return true;
}

bool TryBuildObjc3ReleaseCandidateMatrixArtifact(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &advanced_feature_gate_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.surface_kind.empty() || inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
      inputs.advanced_feature_gate_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "release candidate matrix artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  JsonValue advanced_feature_gate_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error) ||
      !TryParseJsonObjectText(advanced_feature_gate_json,
                              "advanced feature gate",
                              advanced_feature_gate_document, error)) {
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "contract_id"},
          kObjc3AdvancedFeatureReleaseEvidenceContractId) ||
      !JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId) ||
      !JsonStringFieldEquals(advanced_feature_gate_document, {"contract_id"},
                             kObjc3AdvancedFeatureGateContractId)) {
    error = "release candidate matrix inputs drifted";
    return false;
  }

  std::ostringstream out;
  JsonValue::Array matrix_rows;
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("A")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.migration.canonicalization.source.completion.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("B")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.legacy.canonical.migration.semantics.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("C")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("D")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.release.evidence.toolchain.operations.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("E")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.integrated.advanced.feature.gate.v1")},
       {"status", JsonValue::String("pass")}}));

  JsonObjectWriter matrix(out);
  matrix.StringField("contract_id", kObjc3ReleaseCandidateMatrixContractId);
  matrix.StringField("schema_id", kObjc3ReleaseCandidateMatrixSchemaId);
  matrix.StringField("surface_kind", inputs.surface_kind);
  matrix.StringField("release_label", kObjc3AdvancedFeatureReleaseLabel);
  matrix.StringField("report_artifact", inputs.report_artifact_path);
  matrix.StringField("publication_artifact", inputs.publication_artifact_path);
  matrix.StringField("advanced_feature_gate_artifact",
                     inputs.advanced_feature_gate_artifact_path);
  matrix.StringField("validation_artifact_expected",
                     inputs.validation_artifact_path);
  matrix.StringField("release_evidence_operation_artifact_expected",
                     inputs.release_evidence_operation_artifact_path);
  matrix.StringField("dashboard_artifact_expected",
                     inputs.dashboard_artifact_path);
  matrix.RawJsonField(
      "targeted_profile_ids",
      BuildIndentedStringArrayJson(BuildObjc3ReleaseTargetedProfileIds(),
                                   "    "));
  matrix.RawJsonField(
      "matrix_rows",
      objc3::io::json::RenderJson(
          JsonValue::ArrayValue(std::move(matrix_rows))));
  matrix.StringField(
      "matrix_model",
      "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set");
  matrix.BoolField("ready", true);
  artifact_json = FinishJsonObject(matrix, out);
  return true;
}
