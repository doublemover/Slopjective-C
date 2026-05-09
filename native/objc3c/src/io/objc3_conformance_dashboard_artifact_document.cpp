#include "io/objc3_dashboard_status_renderers.h"
#include "io/objc3_process_internal.h"

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

  if (inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
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
  JsonObjectWriter dashboard(out);
  dashboard.StringField("schema_id", kObjc3DashboardStatusSchemaId);
  dashboard.IntField("schema_version", 1);
  dashboard.StringField("dashboard_version", kObjc3DashboardVersion);
  dashboard.StringField("release_label", kObjc3AdvancedFeatureReleaseLabel);
  dashboard.StringField("release_id", kObjc3DashboardReleaseId);
  dashboard.StringField("generated_at", kObjc3DeterministicReplayTimestamp);
  dashboard.StringField("source_revision", kObjc3DeterministicSourceRevision);
  dashboard.StringField("status", "pass");
  dashboard.RawJsonField("profiles", RenderDashboardProfiles());
  dashboard.RawJsonField("dependencies", RenderDashboardDependencies());
  dashboard.RawJsonField(
      "artifacts",
      RenderDashboardArtifacts(inputs, report_json, publication_json,
                               validation_json,
                               release_evidence_operation_json));
  dashboard.RawJsonField("blockers", "[]");
  dashboard.RawJsonField("summary", RenderDashboardSummary());
  dashboard.RawJsonField("refresh", RenderDashboardRefresh());
  dashboard.RawJsonField("change_history", RenderDashboardChangeHistory());
  artifact_json = FinishJsonObject(dashboard, out);
  return true;
}
