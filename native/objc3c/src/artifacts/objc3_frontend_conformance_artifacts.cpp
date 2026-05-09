#include "artifacts/objc3_frontend_conformance_artifacts.h"

#include "artifacts/objc3_frontend_conformance_publication.h"

namespace objc3::artifacts::frontend {

std::string RenderVersionedConformanceReportArtifactJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    unsigned language_version,
    const std::string &runnable_feature_claim_inventory_json,
    const std::string &feature_claim_truth_surface_json,
    const std::string &canonical_selection_claim_semantics_json,
    const std::string &runtime_capability_report_json,
    const std::string &public_conformance_report_json,
    const std::string &advanced_feature_reporting_json,
    const std::string &advanced_feature_release_evidence_json) {
  VersionedConformancePublicationInputs inputs;
  inputs.language_version = language_version;
  inputs.runnable_feature_claim_inventory_json =
      runnable_feature_claim_inventory_json;
  inputs.feature_claim_truth_surface_json = feature_claim_truth_surface_json;
  inputs.canonical_selection_claim_semantics_json =
      canonical_selection_claim_semantics_json;
  inputs.runtime_capability_report_json = runtime_capability_report_json;
  inputs.public_conformance_report_json = public_conformance_report_json;
  inputs.advanced_feature_reporting_json = advanced_feature_reporting_json;
  inputs.advanced_feature_release_evidence_json =
      advanced_feature_release_evidence_json;
  return RenderVersionedConformanceReportPublicationJson(summary, inputs);
}

}  // namespace objc3::artifacts::frontend
