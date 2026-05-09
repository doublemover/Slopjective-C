#include "io/objc3_conformance_release_artifact_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

std::string BuildObjc3ConformanceReportPublicationArtifactDocumentJson(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs) {
  std::ostringstream out;
  JsonObjectWriter publication(out);
  publication.StringField("contract_id", inputs.contract_id);
  publication.StringField("schema_id", inputs.schema_id);
  publication.StringField("selected_profile", inputs.selected_profile);
  publication.BoolField("selected_profile_supported",
                        inputs.selected_profile_supported);
  publication.StringArrayField("supported_profile_ids",
                               inputs.supported_profile_ids);
  publication.StringArrayField("rejected_profile_ids",
                               inputs.rejected_profile_ids);
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
  publication.StringArrayField("advanced_feature_targeted_profile_ids",
                               inputs.advanced_feature_targeted_profile_ids);
  publication.StringField("ci_release_evidence_gate_script_path",
                          inputs.ci_release_evidence_gate_script_path);
  publication.StringField("runbook_reference_path",
                          inputs.runbook_reference_path);
  publication.StringField("dashboard_schema_path",
                          inputs.dashboard_schema_path);
  publication.StringField("report_artifact",
                          inputs.report_artifact_relative_path);
  publication.BoolField("ready", true);
  return FinishJsonObject(publication, out);
}

std::string BuildObjc3ConformanceClaimValidationArtifactDocumentJson(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_schema_id,
    const std::string &report_contract_id,
    const std::string &runtime_capability_contract_id,
    const std::string &public_conformance_schema_id,
    const std::vector<std::string> &advanced_feature_targeted_profile_ids,
    const std::string &ci_release_evidence_gate_script_path,
    const std::string &runbook_reference_path,
    const std::string &dashboard_schema_path,
    const std::string &selected_profile,
    bool selected_profile_supported,
    const std::vector<std::string> &supported_profile_ids,
    const std::vector<std::string> &rejected_profile_ids,
    const std::string &effective_language_profile,
    bool canonical_literal_rejection_diagnostics_enabled,
    const std::string &publication_surface_kind) {
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
  validation.StringArrayField("advanced_feature_targeted_profile_ids",
                              advanced_feature_targeted_profile_ids);
  validation.StringField("ci_release_evidence_gate_script_path",
                         ci_release_evidence_gate_script_path);
  validation.StringField("runbook_reference_path", runbook_reference_path);
  validation.StringField("dashboard_schema_path", dashboard_schema_path);
  validation.StringField("selected_profile", selected_profile);
  validation.BoolField("selected_profile_supported", selected_profile_supported);
  validation.StringArrayField("supported_profile_ids", supported_profile_ids);
  validation.StringArrayField("rejected_profile_ids", rejected_profile_ids);
  validation.StringField("effective_language_profile",
                         effective_language_profile);
  validation.BoolField("canonical_literal_rejection_diagnostics_enabled",
                       canonical_literal_rejection_diagnostics_enabled);
  validation.StringField("publication_surface_kind", publication_surface_kind);
  validation.BoolField("ready", true);
  return FinishJsonObject(validation, out);
}

std::string BuildObjc3ReleaseEvidenceOperationArtifactDocumentJson(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs) {
  std::ostringstream out;
  JsonObjectWriter operation(out);
  operation.StringField("contract_id", kObjc3ReleaseEvidenceOperationContractId);
  operation.StringField("schema_id", kObjc3ReleaseEvidenceOperationSchemaId);
  operation.StringArrayField(
      "dependency_contract_ids",
      {kObjc3AdvancedFeatureOpsContractId,
       kObjc3AdvancedFeatureReleaseEvidenceContractId});
  operation.StringField("validation_contract_id",
                        kObjc3ToolchainConformanceClaimOperationsContractId);
  operation.StringField("dashboard_contract_id",
                        kObjc3DashboardStatusPublicationContractId);
  operation.StringField(
      "operation_model",
      "validation-publishes-release-evidence-command-surface-and-dashboard-status-over-the-final-claim-publication-artifact-set");
  operation.StringField("release_label", kObjc3AdvancedFeatureReleaseLabel);
  operation.StringArrayField(
      "command_tokens",
      {"python", kObjc3AdvancedFeatureEvidenceGateScriptPath});
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
  operation.StringArrayField(
      "targeted_profile_ids",
      {"strict", "strict-concurrency", "strict-system"});
  operation.StringArrayField(
      "corpus_shard_ids",
      {"parser", "semantic", "lowering_abi", "module_roundtrip",
       "diagnostics"});
  operation.StringArrayField(
      "release_evidence_artifact_ids",
      {"EVID-01", "EVID-02", "EVID-03", "EVID-04", "EVID-07",
       "EVID-08", "EVID-09", "EVID-10", "EVID-11"});
  operation.StringField("generated_at", kObjc3DeterministicReplayTimestamp);
  operation.BoolField("ready", true);
  return FinishJsonObject(operation, out);
}

std::string BuildObjc3AdvancedFeatureGateArtifactDocumentJson(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs) {
  std::ostringstream out;
  JsonObjectWriter gate(out);
  gate.StringField("contract_id", kObjc3AdvancedFeatureGateContractId);
  gate.StringField("schema_id", kObjc3AdvancedFeatureGateSchemaId);
  gate.StringArrayField(
      "dependency_contract_ids",
      {"objc3c.tooling.frontend.migration.canonicalization.source.completion.v1",
       "objc3c.tooling.legacy.canonical.migration.semantics.v1",
       kObjc3AdvancedFeatureReleaseEvidenceContractId,
       kObjc3ReleaseEvidenceOperationContractId});
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
  gate.StringArrayField("targeted_profile_ids",
                        BuildObjc3ReleaseTargetedProfileIds());
  gate.BoolField("native_validation_required", true);
  gate.BoolField("report_payload_ready", true);
  gate.BoolField("release_evidence_ready", true);
  gate.BoolField("ready", true);
  return FinishJsonObject(gate, out);
}

std::string BuildObjc3ReleaseCandidateMatrixArtifactDocumentJson(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs) {
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

  std::ostringstream out;
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
  matrix.StringArrayField("targeted_profile_ids",
                          BuildObjc3ReleaseTargetedProfileIds());
  matrix.RawJsonField(
      "matrix_rows",
      objc3::io::json::RenderJson(
          JsonValue::ArrayValue(std::move(matrix_rows))));
  matrix.StringField(
      "matrix_model",
      "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set");
  matrix.BoolField("ready", true);
  return FinishJsonObject(matrix, out);
}
