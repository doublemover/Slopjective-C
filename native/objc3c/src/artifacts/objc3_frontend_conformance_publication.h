#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

struct VersionedConformancePublicationInputs {
  unsigned language_version = 0;
  std::string runnable_feature_claim_inventory_json;
  std::string feature_claim_truth_surface_json;
  std::string canonical_selection_claim_semantics_json;
  std::string runtime_capability_report_json;
  std::string public_conformance_report_json;
  std::string advanced_feature_reporting_json;
  std::string advanced_feature_release_evidence_json;
};

[[nodiscard]] std::string RenderVersionedConformanceReportPublicationJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const VersionedConformancePublicationInputs &inputs);

}  // namespace objc3::artifacts::frontend
