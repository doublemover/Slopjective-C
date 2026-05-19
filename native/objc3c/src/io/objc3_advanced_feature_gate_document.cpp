#include "io/objc3_conformance_release_artifact_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

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
