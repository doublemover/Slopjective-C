#include "io/objc3_conformance_release_artifact_document.h"
#include "io/objc3_process_internal.h"

bool TryBuildObjc3ReleaseEvidenceOperationArtifact(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
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

  artifact_json =
      BuildObjc3ReleaseEvidenceOperationArtifactDocumentJson(inputs);
  return true;
}
