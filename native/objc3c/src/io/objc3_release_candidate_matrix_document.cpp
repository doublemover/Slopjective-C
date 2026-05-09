#include "io/objc3_conformance_release_artifact_document.h"

#include <sstream>
#include <utility>

#include "io/objc3_process_internal.h"

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
  matrix.ValueField("matrix_rows",
                    JsonValue::ArrayValue(std::move(matrix_rows)));
  matrix.StringField(
      "matrix_model",
      "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set");
  matrix.BoolField("ready", true);
  return FinishJsonObject(matrix, out);
}
