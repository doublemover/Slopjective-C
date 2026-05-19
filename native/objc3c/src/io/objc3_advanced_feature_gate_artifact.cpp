#include "io/objc3_conformance_release_artifact_document.h"
#include "io/objc3_process_internal.h"

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

  artifact_json = BuildObjc3AdvancedFeatureGateArtifactDocumentJson(inputs);
  return true;
}
