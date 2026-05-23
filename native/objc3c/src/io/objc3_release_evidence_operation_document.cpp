#include "io/objc3_conformance_release_artifact_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

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
      {"strict-system"});
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
