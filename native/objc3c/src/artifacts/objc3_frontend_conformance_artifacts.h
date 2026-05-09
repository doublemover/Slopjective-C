#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string RenderVersionedConformanceReportArtifactJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    unsigned language_version,
    const std::string &runnable_feature_claim_inventory_json,
    const std::string &feature_claim_truth_surface_json,
    const std::string &canonical_selection_claim_semantics_json,
    const std::string &runtime_capability_report_json,
    const std::string &public_conformance_report_json,
    const std::string &advanced_feature_reporting_json,
    const std::string &advanced_feature_release_evidence_json);

}  // namespace objc3::artifacts::frontend
